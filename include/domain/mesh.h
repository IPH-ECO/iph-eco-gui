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
    double coordinatesDistance;
    QVector<MeshPolygon> domain;

    bool showDomainBoundary;
    bool showMesh;
    bool showUTMCoordinates;
    bool showVertexesLabels;
    bool showTrianglesLabels;
    bool showEdgesLabels;

private:
    Point createNewCoordinate(const Point &p1, const Point &p2, const double &distance);

public:
    Mesh();
    Mesh(QString &_name);
    Mesh(QString &_name, QString &_boundaryFilePath);
    virtual ~Mesh();

    void setName(const QString &name);
    QString getName() const;

    void setBoundaryFilePath(const QString &boundaryFilePath);
    QString getBoundaryFilePath() const;

    void setCoordinatesDistance(const double &coordinatesDistance);
    double getCoordinatesDistance() const;

    QVector<MeshPolygon>& getDomain();
    const MeshPolygon* getBoundaryPolygon();

    void setShowDomainBoundary(const bool &show);
    bool getShowDomainBoundary() const;
    void setShowMesh(const bool &show);
    bool getShowMesh() const;

    double height();
    double width();

    virtual void buildDomain();
    virtual void clear();
};

#endif // MESH_H

