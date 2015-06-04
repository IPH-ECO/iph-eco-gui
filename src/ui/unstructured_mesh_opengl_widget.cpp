#include "include/ui/unstructured_mesh_opengl_widget.h"

#include <QList>
#include <QPoint>

#include "include/utility/cgal_definitions.h"
#include "include/utility/opengl_util.h"

UnstructuredMeshOpenGLWidget::UnstructuredMeshOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    mesh(NULL),
    left(0),
    right(0),
    bottom(0),
    top(0),
    zoom(0),
    movX(0),
    movY(0),
    xAtPress(0),
    yAtPress(0)
{
    this->parent = static_cast<UnstructuredMeshDialog*>(parent);
    setMouseTracking(true);
}

void UnstructuredMeshOpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void UnstructuredMeshOpenGLWidget::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, height, 0.0f, 0.0f, 1.0f);
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

    // left = boundaryPolygon->left_vertex()->x() - zoom;
    // right = boundaryPolygon->right_vertex()->x() + zoom;
    // bottom = boundaryPolygon->bottom_vertex()->y() - zoom;
    // top = boundaryPolygon->top_vertex()->y() + zoom;

    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();

    // glOrtho(left - 50, right + 50, bottom - 50, top + 50, -1.0, 1.0);

    // glTranslatef(movX, movY, 0);

    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();

    // glColor3f(0.0, 0.0, 0.0);

    // if (mesh->getShowDomainBoundary()) {
    //     QList<MeshPolygon> domain = mesh->getDomain();

    //     for (QList<MeshPolygon>::const_iterator it = domain.begin(); it != domain.end(); it++) {
    //         glBegin(GL_LINE_LOOP);
    //         for (MeshPolygonType::Vertex_iterator vt = it->vertices_begin(); vt != it->vertices_end(); vt++) {
    //             glVertex2d(vt->x(), vt->y());
    //         }
    //         glEnd();
    //     }
    // }

    // if (mesh->getShowMesh()) {
    //     const CDT *cdt = mesh->getCDT();

    //     glBegin(GL_LINES);
    //     for (CDT::Finite_faces_iterator fit = cdt->finite_faces_begin(); fit != cdt->finite_faces_end(); ++fit) {
    //         if (!fit->info().isInDomain()) {
    //             continue;
    //         }

    //         for (int i = 0; i < 3; i++) {
    //             CDT::Edge e(fit, i);

    //             if (cdt->is_constrained(e)) {
    //                 continue;
    //             }

    //             Point p1 = e.first->vertex((e.second + 1) % 3)->point();
    //             Point p2 = e.first->vertex((e.second + 2) % 3)->point();

    //             glVertex2f(p1.x(), p1.y());
    //             glVertex2f(p2.x(), p2.y());
    //         }
    //     }
    //     glEnd();
    // }
}

void UnstructuredMeshOpenGLWidget::setMesh(UnstructuredMesh *mesh) {
    this->mesh = mesh;
    update();
}

void UnstructuredMeshOpenGLWidget::buildDomain(const QString &filename) {
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

void UnstructuredMeshOpenGLWidget::generateMesh() {
    try {
        mesh->generate();
        update();
    } catch (MeshException &e) {
        throw e;
    }
}

void UnstructuredMeshOpenGLWidget::wheelEvent(QWheelEvent *event) {
    zoom += event->angleDelta().y() * 2;

    update();
}

void UnstructuredMeshOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {}

void UnstructuredMeshOpenGLWidget::mousePressEvent(QMouseEvent *event) {
//    qDebug() << "press";

    xAtPress = event->x();
    yAtPress = event->y();
}

void UnstructuredMeshOpenGLWidget::mouseReleaseEvent(QMouseEvent *event) {
//    qDebug() << "release";

    movX = (event->x() - xAtPress) * 30;
    movY = (event->y() - yAtPress) * 30;

    update();
}
