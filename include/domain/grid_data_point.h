#ifndef GRID_DATA_POINT_H
#define GRID_DATA_POINT_H

class GridDataPoint {
private:
    double x;
    double y;
    double data;
public:
    GridDataPoint(double x, double y, double data) : x(x), y(y), data(data) {}

    double getX() const {
        return x;
    }

    double getY() const {
        return y;
    }

    double getData() const {
        return data;
    }

    void setData(double &data) {
        this->data = data;
    }
};

#endif // GRID_DATA_POINT_H
