#include "include/services/gridservice.h"

#include <QDebug>

GridService::GridService(QString &_boundaryFilename) :
    boundaryFilename(_boundaryFilename),
    outerBoundaryList(new QList<CoordinateMap>()), innerBoundaryList(new QList<CoordinateMap>()) {}

void GridService::processBoundaryFile() {
    QFile boundaryFile(this->boundaryFilename);

    if (!boundaryFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw GridException(QString("Unable to open boundary file. Error: %1").arg(boundaryFile.errorString()));
    }

    outerBoundaryList->clear();
    innerBoundaryList->clear();

    QXmlStreamReader kml(&boundaryFile);

    while (!kml.atEnd()) {
        kml.readNext();

        if ((kml.name() == "outerBoundaryIs" || kml.name() == "innerBoundaryIs") && kml.isStartElement()) {
            QString boundaryElementName = kml.name().toString();

            do {
                kml.readNext();
            } while (kml.name() != "coordinates");

            QString coordinatesText = kml.readElementText();
            QStringList coordinates = coordinatesText.trimmed().split(" ");
            CoordinateMap coordinateMap;

            for (int i = 0; i < coordinates.count(); i++) {
                QStringList coordinateStr = coordinates.at(i).split(",");
                GeographicLib::GeoCoords coordinate(coordinateStr.at(1).toDouble(), coordinateStr.at(0).toDouble());

                coordinateMap.insert(coordinate.Easting(), coordinate.Northing());
            }

            if (boundaryElementName == "outerBoundaryIs") {
                outerBoundaryList->push_back(coordinateMap);
            } else {
                innerBoundaryList->push_back(coordinateMap);
            }
        }
    }

    if (this->outerBoundaryList->isEmpty()) {
        boundaryFile.close();
        throw GridException("Invalid KML file. No domain coordinates found.");
    }

    boundaryFile.close();
}

QString GridService::getJsonRepresentation() {
    if (this->outerBoundaryList->isEmpty()) {
        throw GridException("No boundary file processing done.");
    }

    QJsonObject boundaryObject;

    boundaryObject["outer"] = this->buildJson(GridService::OUTER_BOUNDARY);
    boundaryObject["inner"] = this->buildJson(GridService::INNER_BOUNDARY);

    return QJsonDocument(boundaryObject).toJson(QJsonDocument::Compact);
}

QJsonArray GridService::buildJson(BoundaryType boundaryType) {
    QString elementName;
    QList<CoordinateMap> *boundaryList;

    if (boundaryType == GridService::OUTER_BOUNDARY) {
        elementName = "outer";
        boundaryList = this->outerBoundaryList;
    } else {
        elementName = "inner";
        boundaryList = this->innerBoundaryList;
        qDebug() << this->innerBoundaryList->size();
    }

    QJsonArray boundaryArrayObject;

    for (int i = 0; i < boundaryList->count(); i++) {
        QJsonArray coordinatesArray;
        CoordinateMap &currentBoundary = const_cast<CoordinateMap&>(boundaryList->at(i));

        for (CoordinateMap::iterator it = currentBoundary.begin(); it != currentBoundary.end(); it++) {
            coordinatesArray.push_back(QString("%1,%2").arg(it.key()).arg(it.value()));
        }

        boundaryArrayObject.push_back(coordinatesArray);
    }

    return boundaryArrayObject;
}

GridService::~GridService() {
    delete outerBoundaryList;
    delete innerBoundaryList;
}
