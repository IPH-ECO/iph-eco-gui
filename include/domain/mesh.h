#ifndef MESH_H
#define MESH_H

#include <QHash>
#include <QJsonObject>
#include <QVector>

#include "mesh_polygon.h"

class Mesh {
protected:
    QString name;
    QString boundaryFilePath;
    QVector<MeshPolygon> domain;

    bool showDomainBoundary;
    bool showMesh;
    bool showUTMCoordinates;
    bool showVertexesLabels;
    bool showTrianglesLabels;
    bool showEdgesLabels;

public:
    Mesh();
    Mesh(QString &_name);
    Mesh(QString &_name, QString &_boundaryFilePath);
    virtual ~Mesh();

    void setName(const QString &name);
    QString getName() const;

    void setBoundaryFilePath(const QString &boundaryFilePath);
    QString getBoundaryFilePath() const;

    void setShowDomainBoundary(const bool &show);
    bool getShowDomainBoundary() const;
    void setShowMesh(const bool &show);
    bool getShowMesh() const;

    void toogleUTMCoordinates(bool &show);
    void toogleVertexesLabels(bool &show);

    virtual void buildDomain();
    QVector<MeshPolygon>& getDomain();
    const MeshPolygon* getBoundaryPolygon();

    virtual void clear();
};

#endif // MESH_H

