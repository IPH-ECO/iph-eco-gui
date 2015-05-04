#ifndef GRID_DATA_OPENGL_WIDGET_H
#define GRID_DATA_OPENGL_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>

#include "include/domain/grid_data_configuration.h"
#include "include/ui/grid_data_dialog.h"

class GridDataOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
private:
    GridDataConfiguration *gridDataConfiguration;
    GridDataDialog *_parent;
    double left;
    double right;
    double bottom;
    double top;

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

public:
    GridDataOpenGLWidget(QWidget *_parent);

    void setGridDataConfiguration(GridDataConfiguration *gridDataConfiguration);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
};

#endif // GRID_DATA_OPENGL_WIDGET_H
