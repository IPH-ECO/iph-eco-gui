#include "include/ui/grid_data_opengl_widget.h"

#include <QList>
#include <boost/numeric/ublas/matrix.hpp>

#include "include/utility/delaunay_triangulation_definitions.h"
#include "include/domain/structured_mesh.h"
#include "include/domain/unstructured_mesh.h"

using boost::numeric::ublas::matrix;

GridDataOpenGLWidget::GridDataOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent), mesh(NULL), left(0), right(0), bottom(0), top(0) {}

void GridDataOpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void GridDataOpenGLWidget::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, height, 0.0f, 0.0f, 1.0f);
}

void GridDataOpenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mesh == NULL) {
        return;
    }

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

    QList<MeshPolygon> &domain = mesh->getDomain();

    for (QList<MeshPolygon>::const_iterator it = domain.begin(); it != domain.end(); it++) {
        glBegin(GL_LINE_LOOP);
        for (MeshPolygon::Vertex_iterator vt = it->vertices_begin(); vt != it->vertices_end(); vt++) {
            glVertex2d(vt->x(), vt->y());
        }
        glEnd();
    }

    if (dynamic_cast<UnstructuredMesh*>(mesh) == NULL) {
        StructuredMesh *structuredMesh = static_cast<StructuredMesh*>(mesh);
        matrix<Polygon> &grid = structuredMesh->getGrid();

        for (ulong i = 0; i < grid.size1(); i++) {
            for (ulong j = 0; j < grid.size2(); j++) {
                Polygon &quad = grid(i, j);

                if (quad.size() == 0) {
                    continue;
                }

                glBegin(GL_LINE_LOOP);
                for (uint k = 0; k < quad.size(); k++) {
                    glVertex2d(quad[k].x(), quad[k].y());
                }
                glEnd();
            }
        }
    } else {
        UnstructuredMesh *unstructuredMesh = static_cast<UnstructuredMesh*>(mesh);
        const CDT *cdt = unstructuredMesh->getCDT();

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

void GridDataOpenGLWidget::setMesh(Mesh *mesh) {
    this->mesh = mesh;
    update();
}
