#ifndef GRID_DATA_POINT_H
#define GRID_DATA_POINT_H

#include <utility/cgal_definitions.h>

class GridDataPoint {
private:
    Point point;
    double data;
public:
    GridDataPoint(const Point &point, const double &data) : point(point), data(data) {}

    const Point& getPoint() const {
        return point;
    }

    double getData() const {
        return data;
    }

    void setData(double &data) {
        this->data = data;
    }
};

#endif // GRID_DATA_POINT_H
