#include "include/services/gridservice.h"

GridService::GridService(QString &_boundaryFilename) : boundaryFilename(_boundaryFilename) {}

QString GridService::getBoundaryJson() {
    QFile boundaryFile(this->boundaryFilename);

    if (!boundaryFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw GridException(QString("Unable to open boundary file. Error: %1").arg(boundaryFile.errorString()));
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

            QString coordinatesText = kml.readElementText();
            QStringList coordinates = coordinatesText.trimmed().split(" ");

            QJsonArray coordinatesJsonArray;

            for (int i = 0; i < coordinates.count(); i++) {
                QStringList coordinateStr = coordinates.at(i).split(",");
                GeographicLib::GeoCoords coordinate(coordinateStr.at(1).toDouble(), coordinateStr.at(0).toDouble());

                coordinatesJsonArray.push_back(QString::number(coordinate.Easting(), 'f', 11) + "," + QString::number(coordinate.Northing(), 'f', 11));
            }

            if (boundaryElementName == "outerBoundaryIs") {
                outerBoundaryJsonArray.push_back(coordinatesJsonArray);
            } else {
                innerBoundaryJsonArray.push_back(coordinatesJsonArray);
            }
        }
    }

    boundaryFile.close();

    if (outerBoundaryJsonArray.isEmpty()) {
        throw GridException("Invalid KML file. No domain coordinates found.");
    }

    QJsonObject boundaryJsonObject;

    boundaryJsonObject["outer"] = outerBoundaryJsonArray;
    boundaryJsonObject["inner"] = innerBoundaryJsonArray;

    return QJsonDocument(boundaryJsonObject).toJson(QJsonDocument::Compact);
}

GridService::~GridService() {}
