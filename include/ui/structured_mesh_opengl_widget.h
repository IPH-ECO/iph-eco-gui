#ifndef STRUCTURED_MESH_OPENGL_WIDGET_H
#define STRUCTURED_MESH_OPENGL_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>

#include "include/utility/cgal_definitions.h"
#include "include/domain/mesh_polygon.h"
#include "include/domain/structured_mesh.h"
#include "include/exceptions/mesh_exception.h"

class StructuredMeshOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
private:
    StructuredMesh *mesh;
    double left;
    double right;
    double bottom;
    double top;
    QMutex mutex;

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public:
    StructuredMeshOpenGLWidget(QWidget *parent);

    void setMesh(StructuredMesh *mesh);
    void buildDomain(const QString &filename);
    void generateMesh();
};

#endif // STRUCTURED_MESH_OPENGL_WIDGET_H

