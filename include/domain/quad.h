#ifndef QUAD_H
#define QUAD_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Point_2.h>
#include <QSet>

#include "include/utility/cgal_definitions.h"
#include "cell_info.h"

class Quad : public Polygon {
private:
    QSet<CellInfo*> cellInfoSet;

public:
    ~Quad();
    QSet<CellInfo*>& getCellInfoSet();
    bool addCellInfo(CellInfo *cellInfo);
    CellInfo* getCellInfo(GridInformationType &gridInformationType);
};

#endif // QUAD_H

