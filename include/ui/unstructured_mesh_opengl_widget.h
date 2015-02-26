#ifndef UNSTRUCTURED_MESH_OPENGL_WIDGET_H
#define UNSTRUCTURED_MESH_OPENGL_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QJsonObject>
#include <QJsonArray>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QVector>
#include <QMutex>
#include <QMutexLocker>

#include "include/utility/delaunay_triangulation_definitions.h"
#include "include/domain/mesh_polygon.h"
#include "include/domain/unstructured_mesh.h"
#include "include/exceptions/mesh_exception.h"

class UnstructuredMeshOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
private:
    UnstructuredMesh *mesh;
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
    UnstructuredMeshOpenGLWidget(QWidget *parent);

    void updateCurrentMesh(UnstructuredMesh *mesh);
    void generateMesh();

    void reset();
    void toogleDomainBoundary(bool show);
};

#endif // UNSTRUCTURED_MESH_OPENGL_WIDGET_H

