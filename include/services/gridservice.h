#ifndef GRIDSERVICE_H
#define GRIDSERVICE_H

#include <QString>
#include <QStringList>
#include <QMultiMap>
#include <QFile>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <GeographicLib/GeoCoords.hpp>

#include "include/exceptions/gridexception.h"

typedef QMultiMap<double, double> CoordinateMap;

class GridService {
private:
    enum BoundaryType { OUTER_BOUNDARY, INNER_BOUNDARY };

    QString boundaryFilename;
    QList<CoordinateMap> *outerBoundaryList;
    QList<CoordinateMap> *innerBoundaryList;

    QJsonArray buildJson(BoundaryType boundaryType);
public:
    GridService(QString &_boundaryFilename);
    ~GridService();

    void processBoundaryFile();
    QString getJsonRepresentation();
};

#endif // GRIDSERVICE_H
