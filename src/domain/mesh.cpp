#include "include/domain/mesh.h"
#include "include/exceptions/mesh_exception.h"

#include <QStringList>
#include <QMultiMap>
#include <QFile>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QJsonArray>

#include <GeographicLib/GeoCoords.hpp>

Mesh::Mesh() : showDomainBoundary(true), showMesh(true) {}

Mesh::Mesh(QString &_name) : name(_name), showMesh(true) {}

Mesh::Mesh(QString &_name, QString &_boundaryFilePath) : name(_name), boundaryFilePath(_boundaryFilePath), showDomainBoundary(true), showMesh(true) {}

Mesh::~Mesh() {}

void Mesh::setName(const QString &name) {
    this->name = name;
}

QString Mesh::getName() const {
    return this->name;
}

void Mesh::setBoundaryFilePath(const QString &boundaryFilePath) {
    this->boundaryFilePath = boundaryFilePath;
}

QString Mesh::getBoundaryFilePath() const {
    return this->boundaryFilePath;
}

void Mesh::setCoordinatesDistance(const double &coordinatesDistance) {
    this->coordinatesDistance = coordinatesDistance;
}

double Mesh::getCoordinatesDistance() const {
    return this->coordinatesDistance;
}

void Mesh::buildDomain() {
    domain.clear();

    QFile boundaryFile(this->getBoundaryFilePath());

    if (!boundaryFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw MeshException(QString("Unable to open boundary file. Error: %1").arg(boundaryFile.errorString()));
    }

    QXmlStreamReader kml(&boundaryFile);

    while (!kml.atEnd()) {
        kml.readNext();

        bool isBoundary = kml.name() == "outerBoundaryIs";
        bool isHole = kml.name() == "innerBoundaryIs";

        if ((isBoundary || isHole) && kml.isStartElement()) {
            do {
                kml.readNext();
            } while (kml.name() != "coordinates" && !kml.atEnd());

            if (kml.atEnd()) {
                throw MeshException(QString("No boundary domain found."));
            }

            // Convert geographic coordinates to UTM coordinates
            QString coordinatesText = kml.readElementText();
            QStringList coordinates = coordinatesText.trimmed().split(" ");
            MeshPolygon meshPolygon(isHole);

            for (int i = 0; i < coordinates.count() - 1; i++) {
                QStringList coordinateStr = coordinates.at(i).split(",");
                GeographicLib::GeoCoords utmCoordinate(coordinateStr.at(1).toDouble(), coordinateStr.at(0).toDouble());

                meshPolygon.push_back(Point(utmCoordinate.Easting(), utmCoordinate.Northing()));
            }

            domain.push_back(meshPolygon);
        }
    }

    boundaryFile.close();
}

QVector<MeshPolygon>& Mesh::getDomain() {
    return domain;
}

const MeshPolygon* Mesh::getBoundaryPolygon() {
    QVector<MeshPolygon>::const_iterator it = std::find_if(domain.begin(), domain.end(), MeshPolygon::isBoundaryPolygon);

    if (it == domain.end()) {
        return NULL;
    }

    return &(*it);
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
    boundaryFilePath.clear();
    domain.clear();
    showDomainBoundary = showMesh = true;
    showUTMCoordinates = showVertexesLabels = showTrianglesLabels = showEdgesLabels = false;
}

double Mesh::height() {
    const MeshPolygon *boundaryPolygon = getBoundaryPolygon();

    if (boundaryPolygon == NULL) {
        return 0.0;
    }

    return boundaryPolygon->top_vertex()->y() - boundaryPolygon->bottom_vertex()->y();
}

double Mesh::width() {
    const MeshPolygon *boundaryPolygon = getBoundaryPolygon();

    if (boundaryPolygon == NULL) {
        return 0.0;
    }

    return boundaryPolygon->right_vertex()->x() - boundaryPolygon->left_vertex()->x();
}
