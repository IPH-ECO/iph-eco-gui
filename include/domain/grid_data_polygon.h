#ifndef GRID_DATA_POLYGON_H
#define GRID_DATA_POLYGON_H

#include "include/utility/delaunay_triangulation_definitions.h"

class GridDataPolygon : public Polygon {
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
