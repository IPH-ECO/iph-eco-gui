#ifndef MESH_H
#define MESH_H

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

class Mesh {
protected:
    QString name;
    QString boundaryFilePath;

private:
    void convertGeoCoordinatesListToUTM(QString &coordinatesText, QJsonArray &coordinatesJsonArray);

public:
    Mesh(QString &_name);
    Mesh(QString &_name, QString &_boundaryFilePath);

    inline bool operator==(const Mesh &mesh) {
        return this->name == mesh.getName();
    }

    void setName(const QString &name);
    QString getName() const;

    void setBoundaryFilePath(const QString &boundaryFilePath);
    QString getBoundaryFilePath() const;

    QString getBoundaryJson();
};

#endif // MESH_H

