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
    MeshPolygon *boundaryPolygon;
    QList<MeshPolygon*> islands;
    QList<MeshPolygon*> refinementAreas;

    bool showDomainBoundary;
    bool showMesh;
    bool showUTMCoordinates;
    bool showVertexesLabels;
    bool showTrianglesLabels;
    bool showEdgesLabels;

private:
    void filterCoordinates(MeshPolygon *meshPolygon);

public:
    Mesh();
    Mesh(QString &_name);
    virtual ~Mesh();

    void setName(const QString &name);
    QString getName() const;
    void setCoordinatesDistance(const double &coordinatesDistance);
    double getCoordinatesDistance() const;
    QList<MeshPolygon*> getIslands();
    MeshPolygon* getBoundaryPolygon();

    MeshPolygon* addMeshPolygon(const QString &filename, const MeshPolygonType &meshPolygonType);
    void removeMeshPolygon(const MeshPolygon &meshPolygon);
    MeshPolygon* getMeshPolygon(const QString &filename, const MeshPolygonType &meshPolygonType);

    void setShowDomainBoundary(const bool &show);
    bool getShowDomainBoundary() const;
    void setShowMesh(const bool &show);
    bool getShowMesh() const;

    double area();

    virtual bool isGenerated() = 0;
    virtual bool instanceOf(const QString &type) = 0;
    virtual void buildDomain(const QString &filename);
    virtual void clear();
};

#endif // MESH_H

