#ifndef MESH_POLYGON_H
#define MESH_POLYGON_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Point_2<K> Point;

class MeshPolygon : public CGAL::Polygon_2<K> {
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
