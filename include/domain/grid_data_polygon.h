#ifndef GRID_DATA_POLYGON_H
#define GRID_DATA_POLYGON_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

class GridDataPolygon : public CGAL::Polygon_2<K> {
private:
    double data;
public:
    GridDataPolygon() {}
    GridDataPolygon(double data) : data(data) {}

    double getData() const {
        return data;
    }

    void setData(const double &data) {
        this->data = data;
    }
};

#endif // GRID_DATA_POLYGON_H
