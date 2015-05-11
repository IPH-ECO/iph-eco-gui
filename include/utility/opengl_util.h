#ifndef OPENGL_UTIL_H
#define OPENGL_UTIL_H

#include <QMouseEvent>
#include "include/utility/cgal_definitions.h"
#include "include/domain/cell_info.h"
#include "include/domain/grid_data.h"

class OpenGLUtil {
public:
    static Point mapCoordinate(QWidget *widget, QMouseEvent *event, double &left, double &right, double &top, double &bottom) {
        int x = event->x(), y = event->y();
        double ratioX = x / (double) widget->width(), ratioY = y / (double) widget->height();
        double mapWidth = right - left, mapHeight = top - bottom;

        return Point(left + mapWidth * ratioX, top - mapHeight * ratioY);
    }

    static double* convertToRGB(const CellInfo *cellInfo) {
        double interval = cellInfo->getGridData()->getMaxValue() - cellInfo->getGridData()->getMinValue();
        double intervalLength = interval / 3.0;
        QVector<double> segments;
        double color[3] = { 0 };
        double weight = cellInfo->getWeight();

        for (double i = 0.0; i < 3.0; i++) {
            segments.push_back(cellInfo->getGridData()->getMinValue() + intervalLength * i);
        }

        if (weight >= segments.at(0) && weight < segments.at(1)) {
            color[0] = weight / interval;
        }
        if (weight >= segments.at(1) && weight < segments.at(2)) {
            color[0] = 1.0;
            color[1] = weight / interval;
        }
        if (weight >= segments.at(2) && weight < cellInfo->getGridData()->getMaxValue()) {
            color[0] = 1.0;
            color[1] = 1.0;
            color[2] = weight / interval;
        }

        return color;
    }
};

#endif // OPENGL_UTIL_H
