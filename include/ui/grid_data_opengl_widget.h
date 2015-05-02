#ifndef GRID_DATA_OPENGL_WIDGET_H
#define GRID_DATA_OPENGL_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>

#include "include/domain/grid_data_configuration.h"

class GridDataOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
private:
    GridDataConfiguration *gridDataConfiguration;
    double left;
    double right;
    double bottom;
    double top;

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

public:
    GridDataOpenGLWidget(QWidget *parent);

    void setGridDataConfiguration(GridDataConfiguration *gridDataConfiguration);
    void mousePressEvent(QMouseEvent *event);
};

#endif // GRID_DATA_OPENGL_WIDGET_H
