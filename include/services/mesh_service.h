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

#include "include/exceptions/mesh_exception.h"

class MeshService {
private:
    QString boundaryFilename;
public:
    MeshService(QString &_boundaryFilename);
    ~MeshService();

    QString getBoundaryJson();
    void convertGeoCoordinatesListToUTM(QString &coordinatesText, QJsonArray &coordinatesJsonArray);
};

#endif // GRIDSERVICE_H
