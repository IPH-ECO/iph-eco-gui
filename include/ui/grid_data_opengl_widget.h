#ifndef GRID_DATA_OPENGL_WIDGET_H
#define GRID_DATA_OPENGL_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include "include/domain/mesh.h"

class GridDataOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
private:
    Mesh *mesh;
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

    void setMesh(Mesh *mesh);
};

#endif // GRID_DATA_OPENGL_WIDGET_H
