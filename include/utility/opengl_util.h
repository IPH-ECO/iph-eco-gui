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

    static QVector<double> convertToRGB(const double &color) {
        QVector<double> rgb;
        double r = color / 10;
        double g = color / 100;
        double b = color / 1000;

        rgb.append(r);
        rgb.append(g);
        rgb.append(b);

        return rgb;
    }
};

#endif // OPENGL_UTIL_H
