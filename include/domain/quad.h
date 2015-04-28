#ifndef QUAD_H
#define QUAD_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Point_2.h>

#include "cell.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Point_2<K> Point;

class Quad : public CGAL::Polygon_2<K> {
private:
    Cell cell;

public:
    void setCell(const Cell &cell) {
        this->cell = cell;
    }

    Cell& getCell() {
        return cell;
    }

    Point centroid() {
        double x = ((*this)[1].x() - (*this)[0].x()) / 2.0;
        double y = ((*this)[3].y() - (*this)[0].y()) / 2.0;
        return Point(x, y);
    }
};

#endif // QUAD_H

