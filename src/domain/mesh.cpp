#include "include/domain/mesh.h"
#include "include/exceptions/mesh_exception.h"

#include <QStringList>
#include <QMultiMap>
#include <QFile>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QJsonArray>

Mesh::Mesh() {}

Mesh::Mesh(QString &_name) : name(_name) {}

Mesh::Mesh(QString &_name, QString &_boundaryFilePath) : name(_name), boundaryFilePath(_boundaryFilePath) {}

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

QJsonObject Mesh::getBoundaryJson() {
    QFile boundaryFile(this->getBoundaryFilePath());

    if (!boundaryFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw MeshException(QString("Unable to open boundary file. Error: %1").arg(boundaryFile.errorString()));
    }

    QXmlStreamReader kml(&boundaryFile);
    QJsonArray domainJsonArray, holesJsonArray;

    while (!kml.atEnd()) {
        kml.readNext();

        if (((kml.name() == "outerBoundaryIs" && domainJsonArray.isEmpty()) || kml.name() == "innerBoundaryIs") && kml.isStartElement()) {
            QString boundaryElementName = kml.name().toString();

            do {
                kml.readNext();
            } while (kml.name() != "coordinates" && !kml.atEnd());

            if (kml.atEnd()) {
                throw MeshException(QString("No boundary domain found."));
            }

            // Convert geographic coordinates to UTM coordinates
            QString coordinatesText = kml.readElementText();
            QJsonArray coordinatesJsonArray;
            QStringList coordinates = coordinatesText.trimmed().split(" ");

            for (int i = 0; i < coordinates.count(); i++) {
                QStringList coordinateStr = coordinates.at(i).split(",");
                GeographicLib::GeoCoords utmCoordinate(coordinateStr.at(1).toDouble(), coordinateStr.at(0).toDouble());
                QJsonObject coordinate;

                coordinate["x"] = utmCoordinate.Easting();
                coordinate["y"] = utmCoordinate.Northing();

                coordinatesJsonArray.push_back(coordinate);
            }

            if (boundaryElementName == "outerBoundaryIs" && domainJsonArray.isEmpty()) {
                domainJsonArray = coordinatesJsonArray;
            } else {
                holesJsonArray.push_back(coordinatesJsonArray);
            }
        }
    }

    boundaryFile.close();

    if (domainJsonArray.isEmpty()) {
        throw MeshException("Invalid KML file. No domain coordinates found.");
    }

    QJsonObject boundaryJsonObject;
    boundaryJsonObject["domain"] = domainJsonArray;
    boundaryJsonObject["holes"] = holesJsonArray;

    return boundaryJsonObject;
}
