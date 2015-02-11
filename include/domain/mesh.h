#ifndef MESH_H
#define MESH_H

#include <QHash>
#include <QJsonObject>

#include <GeographicLib/GeoCoords.hpp>

class Mesh {
protected:
    QString name;
    QString boundaryFilePath;
    bool showUTMCoordinates;
    bool showVertexesLabels;
    bool showTrianglesLabels;
    bool showEdgesLabels;

public:    
    Mesh(QString &_name);
    Mesh(QString &_name, QString &_boundaryFilePath);

    void setName(const QString &name);
    QString getName() const;

    void setBoundaryFilePath(const QString &boundaryFilePath);
    QString getBoundaryFilePath() const;

    void toogleUTMCoordinates(bool &show);
    void toogleVertexesLabels(bool &show);

    QJsonObject getBoundaryJson();
};

inline bool operator==(const Mesh &mesh1, const Mesh &mesh2) {
    return mesh1.getName() == mesh2.getName() && typeid(mesh1) == typeid(mesh2);
}

inline uint qHash(const Mesh &mesh) {
    return qHash(mesh.getName() + typeid(mesh).name());
}

#endif // MESH_H

