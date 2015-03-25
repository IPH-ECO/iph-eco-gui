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

#include <QDebug>

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

            // Get first coordinate
            QStringList coordinateStr = coordinates.at(0).split(",");
            GeographicLib::GeoCoords utmCoordinate(coordinateStr.at(1).toDouble(), coordinateStr.at(0).toDouble());
            Point p1(utmCoordinate.Easting(), utmCoordinate.Northing());

            meshPolygon.push_back(p1);

            for (int i = 1; i < coordinates.count(); i++) {
                coordinateStr = coordinates.at(i).split(",");
                utmCoordinate = GeographicLib::GeoCoords(coordinateStr.at(1).toDouble(), coordinateStr.at(0).toDouble());
                Point p2(utmCoordinate.Easting(), utmCoordinate.Northing());

                if (this->coordinatesDistance > 0.0) {
                    double distance = sqrt(qPow(p2.x() - p1.x(), 2) + qPow(p2.y() - p1.y(), 2));

                    if (distance < this->coordinatesDistance) {
                        continue;
                    }

                    p1 = createNewCoordinate(p1, p2, distance);
                    meshPolygon.push_back(p1);
                    i--;
                } else {
                    meshPolygon.push_back(p2);
                }
            }

            domain.push_back(meshPolygon);
        }
    }

    boundaryFile.close();
}

Point Mesh::createNewCoordinate(const Point &p1, const Point &p2, const double &distance) {
    double x = p1.x() + (p2.x() - p1.x()) * this->coordinatesDistance / distance;
    double y = p1.y() + (p2.y() - p1.y()) * this->coordinatesDistance / distance;

    return Point(x, y);
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
