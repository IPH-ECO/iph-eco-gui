#ifndef DELAUNAY_TRIANGULATION_FACE_H
#define DELAUNAY_TRIANGULATION_FACE_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <QSet>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

#include "cell_info.h"
#include "grid_information_type.h"

class DelaunayTriangulationFace : public CGAL::Delaunay_mesh_face_base_2<K> {
private:
    QSet<CellInfo*> cellInfoSet;
    int nestingLevel;

public:
    DelaunayTriangulationFace();
    ~DelaunayTriangulationFace();
    int getNestingLevel() const;
    void setNestingLevel(const int nestingLevel);
    bool isInDomain();
    CellInfo* getCellInfo(GridInformationType &gridInformationType);
    void addCellInfo(CellInfo *cellInfo);
    QSet<CellInfo*>& getCellInfoSet();
};

#endif // DELAUNAY_TRIANGULATION_FACE_H
