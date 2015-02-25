#include "include/ui/unstructured_mesh_opengl_widget.h"

UnstructuredMeshOpenGLWidget::UnstructuredMeshOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent), mesh(NULL) {}

void UnstructuredMeshOpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void UnstructuredMeshOpenGLWidget::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);
}

void UnstructuredMeshOpenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mesh == NULL) {
        return;
    }

    QMutexLocker locker(&mutex);
    const MeshPolygon *boundaryPolygon = mesh->getBoundaryPolygon();

    if (boundaryPolygon == NULL) {
        return;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(boundaryPolygon->left_vertex()->x(), boundaryPolygon->right_vertex()->x(), boundaryPolygon->bottom_vertex()->y(), boundaryPolygon->top_vertex()->y(), -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    glColor3f(0.0, 0.0, 0.0);
    glPointSize(1.0f);

    if (mesh->getShowDomainBoundary()) {
        QVector<MeshPolygon> domain = mesh->getDomain();

        for (int i = 0; i < domain.count(); i++) {
            MeshPolygon meshPolygon = domain.at(i);

            glBegin(GL_LINE_STRIP);
            for (MeshPolygon::Vertex_iterator vt = meshPolygon.vertices_begin(); vt != meshPolygon.vertices_end(); vt++) {
                glVertex2d(vt->x(), vt->y());
            }
            glEnd();
        }
    }

    if (mesh->getShowMesh()) {
        const CDT *cdt = mesh->getCDT();

        glBegin(GL_LINES);
        for (CDT::Finite_edges_iterator ei = cdt->finite_edges_begin(); ei != cdt->finite_edges_end(); ei++) {
            Point p1 = ei->first->vertex((ei->second + 1) % 3)->point();
            Point p2 = ei->first->vertex((ei->second + 2) % 3)->point();

            glVertex2f(p1.x(), p1.y());
            glVertex2f(p2.x(), p2.y());
        }
        glEnd();
    }
}

void UnstructuredMeshOpenGLWidget::updateCurrentMesh(UnstructuredMesh *_mesh) {
    QMutexLocker locker(&mutex);

    this->mesh = _mesh;

    try {
        this->mesh->buildDomain();
        update();
    } catch (MeshException &e) {
        throw e;
    }
}

void UnstructuredMeshOpenGLWidget::generateMesh() {
    try {
        mesh->generate();
        update();
    } catch (MeshException &e) {
        throw e;
    }
}

void UnstructuredMeshOpenGLWidget::reset() {
    mesh = NULL;
}

void UnstructuredMeshOpenGLWidget::toogleDomainBoundary(bool show) {
    this->mesh->setShowDomainBoundary(show);
}

void UnstructuredMeshOpenGLWidget::wheelEvent(QWheelEvent *event) {
    qDebug() << "zoom in/out";
}

void UnstructuredMeshOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    qDebug() << "move";
}

void UnstructuredMeshOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    qDebug() << "press";
}

void UnstructuredMeshOpenGLWidget::mouseReleaseEvent(QMouseEvent *event) {
    qDebug() << "release";
}
