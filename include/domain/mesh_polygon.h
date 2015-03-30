#ifndef MESH_POLYGON_H
#define MESH_POLYGON_H

#include <QString>

#include "include/utility/delaunay_triangulation_definitions.h"

class MeshPolygon : public Polygon {
private:
    static const double DEFAULT_MINIMUM_ANGLE;
    static const double DEFAULT_MINIMUM_EDGE_LENGTH;

    QString filename;
    int meshPolygonType;
    double minimumAngle;
    double maximumEdgeLength;

public:
    static const QString BOUNDARY_POLYGON_FILENAME;
    static const int BOUNDARY;
    static const int ISLAND;
    static const int REFINEMENT_AREA;

    MeshPolygon();
    MeshPolygon(const QString &filename, int meshPolygonType);

    bool operator==(const MeshPolygon &meshPolygon);

    void setFilename(const QString &filename);
    QString getFilename() const;
    int getMeshPolygonType() const;
    void setMinimumAngle(const double &minimumAngle);
    double getMinimumAngle() const;
    double getMinimumAngleInDegrees() const;
    void setMaximumEdgeLength(const double &maximumEdgeLength);
    double getMaximumEdgeLength() const;
    void setOptimalEdgeLength();

    static inline bool isIsland(MeshPolygon &meshPolygon) {
        return meshPolygon.getMeshPolygonType() == ISLAND;
    }

    static inline bool isBoundary(MeshPolygon &meshPolygon) {
        return meshPolygon.getMeshPolygonType() == BOUNDARY;
    }

    static inline bool isRefinementArea(MeshPolygon &meshPolygon) {
        return meshPolygon.getMeshPolygonType() == REFINEMENT_AREA;
    }

    double width() const;
    double height() const;
};

#endif // MESH_POLYGON_H
