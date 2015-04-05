#include "include/ui/structured_mesh_opengl_widget.h"

#include <QList>
#include <QPoint>

StructuredMeshOpenGLWidget::StructuredMeshOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent), mesh(NULL), left(0), right(0), bottom(0), top(0) {}

void StructuredMeshOpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void StructuredMeshOpenGLWidget::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, height, 0.0f, 0.0f, 1.0f);
}

void StructuredMeshOpenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mesh == NULL) {
        return;
    }

    QMutexLocker locker(&mutex);
    const MeshPolygon *boundaryPolygon = mesh->getBoundaryPolygon();

    if (boundaryPolygon == NULL) {
        return;
    }

    left = boundaryPolygon->left_vertex()->x();
    right = boundaryPolygon->right_vertex()->x();
    bottom = boundaryPolygon->bottom_vertex()->y();
    top = boundaryPolygon->top_vertex()->y();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(left - 50, right + 50, bottom - 50, top + 50, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    glColor3f(0.0, 0.0, 0.0);

    if (mesh->getShowDomainBoundary()) {
        QList<MeshPolygon> domain = mesh->getDomain();

        for (QList<MeshPolygon>::const_iterator it = domain.begin(); it != domain.end(); it++) {
            glBegin(GL_LINE_LOOP);
            for (MeshPolygon::Vertex_iterator vt = it->vertices_begin(); vt != it->vertices_end(); vt++) {
                glVertex2d(vt->x(), vt->y());
            }
            glEnd();
        }
    }

    if (mesh->getShowMesh()) {
        //TODO: Show mesh
    }
}

void StructuredMeshOpenGLWidget::setMesh(StructuredMesh *mesh) {
    this->mesh = mesh;
    update();
}

void StructuredMeshOpenGLWidget::buildDomain(const QString &filename) {
    if (this->mesh == NULL) {
        return;
    }

    try {
        this->mesh->buildDomain(filename);
        update();
    } catch (MeshException &e) {
        throw e;
    }
}

void StructuredMeshOpenGLWidget::generateMesh() {
    try {
        mesh->generate();
        update();
    } catch (MeshException &e) {
        throw e;
    }
}

void StructuredMeshOpenGLWidget::wheelEvent(QWheelEvent *event) {
//    zoom += event->angleDelta().y() * 2;

    update();
}

void StructuredMeshOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
//    qDebug() << "move";
}

void StructuredMeshOpenGLWidget::mousePressEvent(QMouseEvent *event) {
//    qDebug() << "press";
}

void StructuredMeshOpenGLWidget::mouseReleaseEvent(QMouseEvent *event) {
//    qDebug() << "release";
}
