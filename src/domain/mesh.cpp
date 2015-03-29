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
    domain.clear();

    QFile boundaryFile(filename);

    if (!boundaryFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw MeshException(QString("Unable to open boundary file. Error: %1").arg(boundaryFile.errorString()));
    }

    QXmlStreamReader kml(&boundaryFile);

    while (!kml.atEnd()) {
        kml.readNext();

        bool isBoundary = kml.name() == "outerBoundaryIs";
        bool isIsland = kml.name() == "innerBoundaryIs";

        if ((isBoundary || isIsland) && kml.isStartElement()) {
            do {
                kml.readNext();
            } while (kml.name() != "coordinates" && !kml.atEnd());

            if (kml.atEnd()) {
                throw MeshException(QString("No coordinates found in domain file."));
            }

            // Convert geographic coordinates to UTM coordinates
            QString coordinatesText = kml.readElementText();
            QStringList coordinates = coordinatesText.trimmed().split(" ");
            MeshPolygon::MeshPolygonType meshPolygonType = isBoundary ? MeshPolygon::BOUNDARY : MeshPolygon::ISLAND;
            MeshPolygon meshPolygon(MeshPolygon::BOUNDARY_POLYGON_FILENAME, meshPolygonType);

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

void Mesh::addIsland(const QString &filename) {
    QFile islandFile(filename);

    if (!islandFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw MeshException(QString("Unable to open island file. Error: %1").arg(islandFile.errorString()));
    }

    QXmlStreamReader kml(&islandFile);

    while (!kml.atEnd()) {
        kml.readNext();

        if (kml.name() == "outerBoundaryIs" && kml.isStartElement()) {
            do {
                kml.readNext();
            } while (kml.name() != "coordinates" && !kml.atEnd());

            if (kml.atEnd()) {
                throw MeshException(QString("No coordinates found in domain file."));
            }

            QString coordinatesText = kml.readElementText();
            QStringList coordinates = coordinatesText.trimmed().split(" ");
            MeshPolygon islandPolygon(filename, MeshPolygon::ISLAND);

            for (int i = 0; i < coordinates.count(); i++) {
                QStringList coordinateStr = coordinates.at(i).split(",");
                GeographicLib::GeoCoords utmCoordinate = GeographicLib::GeoCoords(coordinateStr.at(1).toDouble(), coordinateStr.at(0).toDouble());
                Point p(utmCoordinate.Easting(), utmCoordinate.Northing());

                islandPolygon.push_back(p);
            }

            domain.push_back(islandPolygon);
        }
    }
}

void Mesh::removeIsland(const QString &filename) {
    for (int i = 0; i < domain.size(); i++) {
        if (domain.at(i).getFilename() == filename) {
            domain.remove(i);
            break;
        }
    }
}

Point Mesh::createNewCoordinate(const Point &p1, const Point &p2, const double &distance) {
    double x = p1.x() + (p2.x() - p1.x()) * this->coordinatesDistance / distance;
    double y = p1.y() + (p2.y() - p1.y()) * this->coordinatesDistance / distance;

    return Point(x, y);
}

QVector<MeshPolygon>& Mesh::getDomain() {
    return domain;
}

MeshPolygon* Mesh::getBoundaryPolygon() {
    MeshPolygon meshPolygon(MeshPolygon::BOUNDARY_POLYGON_FILENAME, MeshPolygon::BOUNDARY);
    QVector<MeshPolygon>::iterator it = std::find(domain.begin(), domain.end(), meshPolygon);

    if (it == domain.end()) {
        return NULL;
    }

    return &(*it);
}

void Mesh::setBoundaryPolygon(const MeshPolygon &meshPolygon) {
    domain.push_back(meshPolygon);
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
    double area = getBoundaryPolygon()->area();

    for (int i = 0; i < domain.count(); i++) {
        const MeshPolygon &innerPolygon = domain.at(i);

        if (innerPolygon.getMeshPolygonType() == MeshPolygon::ISLAND) {
            area -= innerPolygon.area();
        }
    }

    return area;
}
