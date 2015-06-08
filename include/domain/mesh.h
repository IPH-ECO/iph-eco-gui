#ifndef MESH_H
#define MESH_H

#include <QHash>
#include <QJsonObject>
#include <QList>
#include <vtkPolyData.h>
#include "mesh_polygon.h"

class Mesh {
protected:
    QString name;
    double coordinatesDistance;
    MeshPolygon *boundaryPolygon;
    QList<MeshPolygon*> islands;
    QList<MeshPolygon*> refinementAreas;

    // Transient attributes
    bool showBoundaryEdges;
    bool showMesh;
    bool showUTMCoordinates;

private:
    void filterCoordinates(MeshPolygon *meshPolygon);

public:
    Mesh();
    virtual ~Mesh();

    void setName(const QString &name);
    QString getName() const;
    void setCoordinatesDistance(const double &coordinatesDistance);
    double getCoordinatesDistance() const;
    MeshPolygon* getBoundaryPolygon();
    QList<MeshPolygon*> getIslands();
    QList<MeshPolygon*> getRefinementAreas();

    MeshPolygon* addMeshPolygon(const QString &filename, const MeshPolygonType &meshPolygonType);
    void removeMeshPolygon(const QString &filename, const MeshPolygonType &meshPolygonType);
    MeshPolygon* getMeshPolygon(const QString &filename, const MeshPolygonType &meshPolygonType);

    void setShowBoundaryEdges(const bool &toogle);
    bool getShowBoundaryEdges() const;
    void setShowMesh(const bool &toogle);
    bool getShowMesh() const;

    double area();

    virtual bool instanceOf(const QString &type) = 0;
    virtual vtkPolyData* getGrid() = 0;
    virtual void generate() = 0;
    virtual void clear();
};

#endif // MESH_H

