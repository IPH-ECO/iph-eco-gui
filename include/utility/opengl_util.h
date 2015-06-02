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
        double ratioX = x / widget->width(), ratioY = y / widget->height();
        double mapWidth = right - left, mapHeight = top - bottom;

        return Point(left + mapWidth * ratioX, top - mapHeight * ratioY);
    }

    static QVector<double> convertToRGB(const CellInfo *cellInfo) {
        double minValue = cellInfo->getGridData()->getMinValue();
        double maxValue = cellInfo->getGridData()->getMaxValue();

        if (minValue < 0) {
            maxValue += abs(minValue);
            minValue = 0;
        }

        double interval = maxValue - minValue, segmentLength = interval / 3.0;
        QVector<double> segments(4), color(3, 1.0);
        double weight = cellInfo->getWeight();

        for (double i = 0.0; i < 3.0; i++) {
            segments.push_back(minValue + segmentLength * i);
        }
        segments.push_back(maxValue);

        if (weight >= segments.at(0) && weight < segments.at(1)) {
            color[0] = weight / interval;
        }
        if (weight >= segments.at(1) && weight < segments.at(2)) {
            color[0] = 0.0;
            color[1] = weight / interval;
        }
        if (weight >= segments.at(2) && weight <= segments.at(3)) {
            color[0] = 0.0;
            color[1] = 0.0;
            color[2] = weight / interval;
        }

        return color;
    }
};

#endif // OPENGL_UTIL_H
