#ifndef QUAD_H
#define QUAD_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Point_2.h>
#include <QSet>

#include "cell_info.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Point_2<K> Point;

class Quad : public CGAL::Polygon_2<K> {
private:
    QSet<CellInfo*> cellInfoSet;

public:
    QSet<CellInfo*> getCellInfoSet() {
        return cellInfoSet;
    }

    void addCellInfo(CellInfo *cellInfo) {
        cellInfoSet.insert(cellInfo);
    }

    CellInfo* getCellInfo(GridInformationType &gridInformationType) {
        for (QSet<CellInfo*>::const_iterator it = cellInfoSet.begin(); it != cellInfoSet.end(); it++) {
            if ((*it)->getGridInformationType().getValue() == gridInformationType.getValue()) {
                return *it;
            }
        }

        return NULL;
    }

    Point centroid() {
        if (this->size() != 4) {
            //TODO: throw exception
        }

        double x = ((*this)[1].x() - (*this)[0].x()) / 2.0;
        double y = ((*this)[3].y() - (*this)[0].y()) / 2.0;

        return Point(x, y);
    }
};

#endif // QUAD_H

