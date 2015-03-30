#ifndef MESH_POLYGON_H
#define MESH_POLYGON_H

#include <QString>

#include "include/utility/delaunay_triangulation_definitions.h"

class MeshPolygon : public Polygon {
public:
    enum MeshPolygonType { BOUNDARY, ISLAND, REFINEMENT_AREA };

private:
    static const double DEFAULT_MINIMUM_ANGLE;
    static const double DEFAULT_MINIMUM_EDGE_LENGTH;

    QString filename;
    MeshPolygonType meshPolygonType;
    double minimumAngle;
    double maximumEdgeLength;

public:
    static const QString BOUNDARY_POLYGON_FILENAME;

    MeshPolygon();
    MeshPolygon(const QString &filename, MeshPolygonType meshPolygonType);

    bool operator==(const MeshPolygon &meshPolygon);

    void setFilename(const QString &filename);
    QString getFilename() const;
    MeshPolygonType getMeshPolygonType() const;
    void setMinimumAngle(const double &minimumAngle);
    double getMinimumAngle() const;
    double getMinimumAngleInDegrees() const;
    void setMaximumEdgeLength(const double &maximumEdgeLength);
    double getMaximumEdgeLength() const;
    void setOptimalEdgeLength();

    bool isBoundary() const;
    bool isIsland() const;
    bool isRefinementArea() const;

    double width() const;
    double height() const;
};

#endif // MESH_POLYGON_H
