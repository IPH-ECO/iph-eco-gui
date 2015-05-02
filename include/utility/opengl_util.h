#ifndef OPENGL_UTIL_H
#define OPENGL_UTIL_H

#include <QMouseEvent>
#include "include/utility/cgal_definitions.h"

class OpenGLUtil {
public:
    static Point mapCoordinate(QWidget *widget, QMouseEvent *event, double &left, double &right, double &top, double &bottom) {
        int x = event->x(), y = event->y();
        double ratioX = x / (double) widget->width(), ratioY = y / (double) widget->height();
        double mapWidth = right - left, mapHeight = top - bottom;

        return Point(left + mapWidth * ratioX, top - mapHeight * ratioY);
    }
};

#endif // OPENGL_UTIL_H
