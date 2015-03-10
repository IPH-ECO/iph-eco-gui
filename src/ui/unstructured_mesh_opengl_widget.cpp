#include "include/ui/unstructured_mesh_opengl_widget.h"

#include <QPoint>

UnstructuredMeshOpenGLWidget::UnstructuredMeshOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent), mesh(NULL), zoom_factor_base(1.0) {}

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

    double lx = boundaryPolygon->left_vertex()->x() + zoom_factor_base;
    double rx = boundaryPolygon->right_vertex()->x() - zoom_factor_base;
    double by = boundaryPolygon->bottom_vertex()->y() + zoom_factor_base;
    double ty = boundaryPolygon->top_vertex()->y() - zoom_factor_base;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(lx, rx, by, ty, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    glColor3f(0.0, 0.0, 0.0);

    if (mesh->getShowDomainBoundary()) {
        QVector<MeshPolygon> domain = mesh->getDomain();

        for (int i = 0; i < domain.count(); i++) {
            MeshPolygon meshPolygon = domain.at(i);

            glBegin(GL_LINE_LOOP);
            for (MeshPolygon::Vertex_iterator vt = meshPolygon.vertices_begin(); vt != meshPolygon.vertices_end(); vt++) {
                glVertex2d(vt->x(), vt->y());
            }
            glEnd();
        }
    }

    if (mesh->getShowMesh()) {
        const CDT *cdt = mesh->getCDT();

        glBegin(GL_LINES);
        for (CDT::Finite_faces_iterator fit = cdt->finite_faces_begin(); fit != cdt->finite_faces_end(); ++fit) {
            if (!fit->info().in_domain()) {
                continue;
            }

            for (int i = 0; i < 3; i++) {
                CDT::Edge e(fit, i);

                if (cdt->is_constrained(e)) {
                    continue;
                }

                Point p1 = e.first->vertex((e.second + 1) % 3)->point();
                Point p2 = e.first->vertex((e.second + 2) % 3)->point();

                glVertex2f(p1.x(), p1.y());
                glVertex2f(p2.x(), p2.y());
            }
        }
        glEnd();
    }
}

void UnstructuredMeshOpenGLWidget::setMesh(UnstructuredMesh *mesh) {
    this->mesh = mesh;
}

void UnstructuredMeshOpenGLWidget::generateDomain() {
    if (this->mesh == NULL) {
        return;
    }

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

void UnstructuredMeshOpenGLWidget::wheelEvent(QWheelEvent *event) {
    QPoint angleDelta = event->angleDelta();
    if (!angleDelta.isNull() && angleDelta.y() != 0 && angleDelta.x() == 0) {
        zoom_factor_base += angleDelta.y() * 2;
        update();
    }
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
