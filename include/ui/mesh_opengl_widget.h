#ifndef MESH_OPENGL_WIDGET_H
#define MESH_OPENGL_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QJsonObject>
#include <QJsonArray>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPointF>
#include <QDebug>
#include <QVector>

#include "include/domain/mesh.h"

class MeshOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
private:
    Mesh mesh;
    QVector<QPointF> domainCoordinates; // Temporary point structure. CGAL Point representation will be used.
    QVector<QVector<QPointF> > holes;
    double minWidth;
    double maxWidth;
    double minHeight;
    double maxHeight;

    void parseDomainCoordinates(QJsonArray &jsonCoordinates);
    void parseHoleCoordinates(QJsonArray &jsonCoordinates);

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public:
    MeshOpenGLWidget(QWidget *parent);

    void showDomain(const Mesh &mesh);
    void clearMap();
    void toogleBoundaryDomain(bool show);
};

#endif // MESH_OPENGL_WIDGET_H

