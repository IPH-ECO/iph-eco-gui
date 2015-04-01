#ifndef MESH_H
#define MESH_H

#include <QHash>
#include <QJsonObject>
#include <QList>

#include "mesh_polygon.h"

class Mesh {
protected:
    QString name;
    double coordinatesDistance;
    QList<MeshPolygon> domain;

    bool showDomainBoundary;
    bool showMesh;
    bool showUTMCoordinates;
    bool showVertexesLabels;
    bool showTrianglesLabels;
    bool showEdgesLabels;

private:
    void filterCoordinates(MeshPolygon &meshPolygon);
    void buildMeshPolygon(MeshPolygon &meshPolygon);

public:
    Mesh();
    Mesh(QString &_name);
    virtual ~Mesh();

    void setName(const QString &name);
    QString getName() const;

    void setCoordinatesDistance(const double &coordinatesDistance);
    double getCoordinatesDistance() const;

    QList<MeshPolygon>& getDomain();
    QList<MeshPolygon*> getIslands();
    MeshPolygon* getBoundaryPolygon();
    void setBoundaryPolygon(const MeshPolygon &meshPolygon);

    MeshPolygon* addMeshPolygon(MeshPolygon &meshPolygon);
    void removeMeshPolygon(const MeshPolygon &meshPolygon);
    MeshPolygon* getMeshPolygon(const MeshPolygon &meshPolygon);

    void setShowDomainBoundary(const bool &show);
    bool getShowDomainBoundary() const;
    void setShowMesh(const bool &show);
    bool getShowMesh() const;

    double area();

    virtual void buildDomain(const QString &filename);
    virtual void clear();
};

#endif // MESH_H

