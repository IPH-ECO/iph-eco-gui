#ifndef DELAUNAY_TRIANGULATION_DEFINITIONS_H
#define DELAUNAY_TRIANGULATION_DEFINITIONS_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Constrained_triangulation_face_base_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Polygon_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

struct FaceInfo : public CGAL::Delaunay_mesh_face_base_2<K> {
    FaceInfo() {}

    int nesting_level;

    bool in_domain() {
        return nesting_level % 2 == 1; //positive odd
    }
};

typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo, K> Fbi;
typedef CGAL::Delaunay_mesh_face_base_2<K, Fbi> Fbb;
typedef CGAL::Constrained_triangulation_face_base_2<K, Fbb> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
typedef CGAL::Delaunay_mesher_2<CDT, Criteria> Mesher;
typedef CGAL::Polygon_2<K> Polygon;
typedef CDT::Point Point;

#endif // DELAUNAY_TRIANGULATION_DEFINITIONS_H
