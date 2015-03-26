#ifndef MESH_POLYGON_H
#define MESH_POLYGON_H

#include "include/utility/delaunay_triangulation_definitions.h"

class MeshPolygon : public Polygon {
private:
    bool hole;
public:
    MeshPolygon(bool hole = false);
    void setHole(const bool &hole);
    bool isHole() const;

    static bool isHolePolygon(const MeshPolygon &meshPolygon) {
        return meshPolygon.isHole();
    }

    static bool isBoundaryPolygon(const MeshPolygon &meshPolygon) {
        return !isHolePolygon(meshPolygon);
    }
};

#endif // MESH_POLYGON_H
