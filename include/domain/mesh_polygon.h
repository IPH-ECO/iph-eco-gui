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

    MeshPolygon& operator=(const MeshPolygon &meshPolygon);
    bool operator==(const MeshPolygon &meshPolygon);

    void setFilename(const QString &filename);
    QString getFilename() const;
    MeshPolygonType getMeshPolygonType() const;
    void setMinimumAngle(const double &minimumAngle);
    double getMinimumAngle() const;
    double getMinimumAngleInCGALRepresentation() const;
    void setMaximumEdgeLength(const double &maximumEdgeLength);
    double getMaximumEdgeLength() const;
    void setOptimalEdgeLength();

    double width() const;
    double height() const;

    inline bool isBoundary() const {
        return this->meshPolygonType == MeshPolygon::BOUNDARY;
    }

    inline bool isIsland() const {
        return this->meshPolygonType == MeshPolygon::ISLAND;
    }

    inline bool isRefinementArea() const {
        return this->meshPolygonType == MeshPolygon::REFINEMENT_AREA;
    }
};

#endif // MESH_POLYGON_H
