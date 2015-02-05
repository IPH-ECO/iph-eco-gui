#include "include/services/mesh_service.h"

MeshService::MeshService(QString &_boundaryFilename) : boundaryFilename(_boundaryFilename) {}

QString MeshService::getBoundaryJson() {
    QFile boundaryFile(this->boundaryFilename);

    if (!boundaryFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw MeshException(QString("Unable to open boundary file. Error: %1").arg(boundaryFile.errorString()));
    }

    QXmlStreamReader kml(&boundaryFile);
    QJsonArray outerBoundaryJsonArray, innerBoundaryJsonArray;

    while (!kml.atEnd()) {
        kml.readNext();

        if ((kml.name() == "outerBoundaryIs" || kml.name() == "innerBoundaryIs") && kml.isStartElement()) {
            QString boundaryElementName = kml.name().toString();

            do {
                kml.readNext();
            } while (kml.name() != "coordinates");

            if (kml.atEnd()) {
                throw MeshException(QString("No boundary domain found."));
            }

            QString coordinatesText = kml.readElementText();
            QJsonArray coordinatesJsonArray;

            convertGeoCoordinatesListToUTM(coordinatesText, coordinatesJsonArray);

            if (boundaryElementName == "outerBoundaryIs" && outerBoundaryJsonArray.isEmpty()) {
                outerBoundaryJsonArray = coordinatesJsonArray;
            } else {
                innerBoundaryJsonArray.push_back(coordinatesJsonArray);
            }
        }
    }

    boundaryFile.close();

    if (outerBoundaryJsonArray.isEmpty()) {
        throw MeshException("Invalid KML file. No domain coordinates found.");
    }

    QJsonObject boundaryJsonObject;

    boundaryJsonObject["outer"] = outerBoundaryJsonArray;
    boundaryJsonObject["inner"] = innerBoundaryJsonArray;

    return QJsonDocument(boundaryJsonObject).toJson(QJsonDocument::Compact);
}

void MeshService::convertGeoCoordinatesListToUTM(QString &coordinatesText, QJsonArray &coordinatesJsonArray) {
    QStringList coordinates = coordinatesText.trimmed().split(" ");

    for (int i = 0; i < coordinates.count(); i++) {
        QStringList coordinateStr = coordinates.at(i).split(",");
        GeographicLib::GeoCoords coordinate(coordinateStr.at(1).toDouble(), coordinateStr.at(0).toDouble());

        coordinatesJsonArray.push_back(QString::number(coordinate.Easting(), 'f', 11) + "," + QString::number(coordinate.Northing(), 'f', 11));
    }
}

MeshService::~MeshService() {}
