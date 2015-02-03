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

#include "include/exceptions/grid_exception.h"

class GridService {
private:
    QString boundaryFilename;
public:
    GridService(QString &_boundaryFilename);
    ~GridService();

    QString getBoundaryJson();
};

#endif // GRIDSERVICE_H
