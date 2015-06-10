#ifndef DELAUNAY_TRIANGULATION_FACE_H
#define DELAUNAY_TRIANGULATION_FACE_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

class DelaunayTriangulationFace : public CGAL::Delaunay_mesh_face_base_2<K> {
private:
    int nestingLevel;

public:
    DelaunayTriangulationFace();
    ~DelaunayTriangulationFace();
    int getNestingLevel() const;
    void setNestingLevel(const int nestingLevel);
    bool isInDomain();
};

#endif // DELAUNAY_TRIANGULATION_FACE_H
