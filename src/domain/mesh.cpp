#include "include/domain/mesh.h"
#include "include/exceptions/mesh_exception.h"

#include <QStringList>
#include <QMultiMap>
#include <QFile>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QJsonArray>
#include <QtMath>

#include <GeographicLib/GeoCoords.hpp>

#include <QDebug>

Mesh::Mesh() : coordinatesDistance(0.0), showDomainBoundary(true), showMesh(true) {}

Mesh::Mesh(QString &_name) : name(_name), coordinatesDistance(0.0), showDomainBoundary(true) {}

Mesh::~Mesh() {}

void Mesh::setName(const QString &name) {
    this->name = name;
}

QString Mesh::getName() const {
    return this->name;
}

void Mesh::setCoordinatesDistance(const double &coordinatesDistance) {
    this->coordinatesDistance = coordinatesDistance;
}

double Mesh::getCoordinatesDistance() const {
    return this->coordinatesDistance;
}

void Mesh::buildDomain(const QString &filename) {
    MeshPolygon *boundaryMeshPolygon = getBoundaryPolygon();

    if (boundaryMeshPolygon == NULL) {
        MeshPolygon meshPolygon(filename, MeshPolygon::BOUNDARY);
        boundaryMeshPolygon = addMeshPolygon(meshPolygon);
    } else {
        boundaryMeshPolygon->setFilename(filename);
        buildMeshPolygon(*boundaryMeshPolygon);
    }

    QList<MeshPolygon*> islands = this->getIslands();

    for (QList<MeshPolygon*>::iterator it = islands.begin(); it != islands.end(); it++) {
        buildMeshPolygon(*(*it));
    }
}

void Mesh::buildMeshPolygon(MeshPolygon &meshPolygon) {
    QString filename = meshPolygon.getFilename();
    QFile kmlFile(filename);

    if (!kmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw MeshException(QString("Unable to open KML file. Error: %1").arg(kmlFile.errorString()));
    }

    QXmlStreamReader kmlReader(&kmlFile);

    meshPolygon.clear();

    while (!kmlReader.atEnd()) {
        kmlReader.readNext();

        if (kmlReader.name() == "outerBoundaryIs" && kmlReader.isStartElement()) {
            do {
                kmlReader.readNext();
            } while (kmlReader.name() != "coordinates" && !kmlReader.atEnd());

            if (kmlReader.atEnd()) {
                throw MeshException(QString("No coordinates found in KML file."));
            }

            QString coordinatesText = kmlReader.readElementText();
            QStringList coordinates = coordinatesText.trimmed().split(" ");

            for (int i = 0; i < coordinates.count(); i++) {
                QStringList coordinateStr = coordinates.at(i).split(",");
                GeographicLib::GeoCoords utmCoordinate(coordinateStr.at(1).toDouble(), coordinateStr.at(0).toDouble());
                Point p(utmCoordinate.Easting(), utmCoordinate.Northing());

                meshPolygon.push_back(p);
            }

            break;
        }
    }

    kmlFile.close();

    if (this->coordinatesDistance > 0.0) {
        filterCoordinates(meshPolygon);
    }
}

void Mesh::filterCoordinates(MeshPolygon &meshPolygon) {
    MeshPolygon::Vertex_iterator vit = meshPolygon.vertices_begin();
    Point p1 = *vit++;

    while (vit != meshPolygon.vertices_end()) {
        Point p2 = *vit;
        double distance = sqrt(qPow(p2.x() - p1.x(), 2) + qPow(p2.y() - p1.y(), 2));

        if (distance < this->coordinatesDistance) {
            meshPolygon.erase(vit);
        } else {
            double x = p1.x() + (p2.x() - p1.x()) * this->coordinatesDistance / distance;
            double y = p1.y() + (p2.y() - p1.y()) * this->coordinatesDistance / distance;

            p1 = Point(x, y);
            vit = meshPolygon.insert(vit + 1, p1);
        }
    }
}

MeshPolygon* Mesh::addMeshPolygon(MeshPolygon &meshPolygon) {
    buildMeshPolygon(meshPolygon);
    domain.append(meshPolygon);

    return &(domain.last());
}

QList<MeshPolygon*> Mesh::getIslands() {
    QList<MeshPolygon*> islands;

    for (QList<MeshPolygon>::iterator it = domain.begin(); it != domain.end(); it++) {
        if (it->isIsland()) {
            islands.append(&(*it));
        }
    }

    return islands;
}

void Mesh::removeMeshPolygon(const MeshPolygon &meshPolygon) {
    domain.removeOne(meshPolygon);
}

QList<MeshPolygon>& Mesh::getDomain() {
    return domain;
}

void Mesh::setDomain(const QList<MeshPolygon> &domain) {
    this->domain = domain;
}

MeshPolygon* Mesh::getBoundaryPolygon() {
    for (QList<MeshPolygon>::iterator it = domain.begin(); it != domain.end(); it++) {
        if (it->isBoundary()) {
            return &(*it);
        }
    }

    return NULL;
}

MeshPolygon* Mesh::getMeshPolygon(const MeshPolygon &meshPolygon) {
    QList<MeshPolygon>::iterator it = qFind(domain.begin(), domain.end(), meshPolygon);

    if (it == domain.end()) {
        return NULL;
    }

    return &(*it);
}

void Mesh::setBoundaryPolygon(const MeshPolygon &meshPolygon) {
    domain.append(meshPolygon);
}

void Mesh::setShowDomainBoundary(const bool &show) {
    this->showDomainBoundary = show;
}

bool Mesh::getShowDomainBoundary() const {
    return this->showDomainBoundary;
}

void Mesh::setShowMesh(const bool &show) {
    this->showMesh = show;
}

bool Mesh::getShowMesh() const {
    return this->showMesh;
}

void Mesh::clear() {
    name.clear();
    domain.clear();
    showDomainBoundary = showMesh = true;
    showUTMCoordinates = showVertexesLabels = showTrianglesLabels = showEdgesLabels = false;
}

double Mesh::area() {
    double area = 0;

    for (QList<MeshPolygon>::const_iterator it = domain.begin(); it != domain.end(); it++) {
        if (it->isBoundary()) {
            area += it->area();
        } else {
            if (it->isIsland()) {
                area -= it->area();
            }
        }
    }

    return area;
}
