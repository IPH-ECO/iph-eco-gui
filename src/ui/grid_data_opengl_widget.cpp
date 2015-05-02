#include "include/ui/grid_data_opengl_widget.h"

#include <QList>
#include <boost/numeric/ublas/matrix.hpp>

#include "include/ui/cell_update_dialog.h"
#include "include/utility/cgal_definitions.h"
#include "include/domain/structured_mesh.h"
#include "include/domain/unstructured_mesh.h"

#include <QDebug>

using boost::numeric::ublas::matrix;

GridDataOpenGLWidget::GridDataOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent), gridDataConfiguration(NULL), left(0), right(0), bottom(0), top(0) {}

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

    if (gridDataConfiguration == NULL) {
        return;
    }

    Mesh *mesh = gridDataConfiguration->getMesh();

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

    if (gridDataConfiguration->getShowMesh()) {
        if (dynamic_cast<UnstructuredMesh*>(mesh) == NULL) {
            StructuredMesh *structuredMesh = static_cast<StructuredMesh*>(mesh);
            matrix<Quad*> &grid = structuredMesh->getGrid();

            for (ulong i = 0; i < grid.size1(); i++) {
                for (ulong j = 0; j < grid.size2(); j++) {
                    Quad *quad = grid(i, j);

                    if (quad == NULL || quad->is_empty()) {
                        continue;
                    }

                    glBegin(GL_LINE_LOOP);
                    for (uint k = 0; k < quad->size(); k++) {
                        Point point = (*quad)[k];
                        glVertex2d(point.x(), point.y());
                    }
                    glEnd();
                }
            }
        } else {
            UnstructuredMesh *unstructuredMesh = static_cast<UnstructuredMesh*>(mesh);
            const CDT *cdt = unstructuredMesh->getCDT();

            glBegin(GL_LINES);
            for (CDT::Finite_faces_iterator fit = cdt->finite_faces_begin(); fit != cdt->finite_faces_end(); ++fit) {
                if (!fit->info().isInDomain()) {
                    continue;
                }

                for (int i = 0; i < 3; i++) {
                    CDT::Edge e(fit, i);
                    Point p1 = e.first->vertex((e.second + 1) % 3)->point();
                    Point p2 = e.first->vertex((e.second + 2) % 3)->point();

                    glVertex2f(p1.x(), p1.y());
                    glVertex2f(p2.x(), p2.y());
                }
            }
            glEnd();
        }

        QVector<GridData*> &gridDataVector = gridDataConfiguration->getGridDataVector();

        for (int i = 0; i < gridDataVector.count(); i++) {
            GridData *gridData = gridDataVector.at(i);

//            if (gridData->getShow()) {
            if (false) {
                if (gridData->getGridInputType() == GridData::POINT) {
                    QSet<GridDataPoint> &dataPoints = gridData->getGridDataPoints();

                    //TODO: Refactor loop
                    glBegin(GL_POINTS);
                    for (QSet<GridDataPoint>::const_iterator it = dataPoints.begin(); it != dataPoints.end(); it++) {
                        glVertex2f(it->getPoint().x(), it->getPoint().y());
                    }
                    glEnd();
                } else {
                    GridDataPolygon &dataPolygon = gridData->getGridDataPolygon();

                    glBegin(GL_LINE_LOOP);
                    for (ulong j = 0; j < dataPolygon.size(); j++) {
                        Point point = dataPolygon[j];
                        glVertex2f(point.x(), point.y());
                    }
                    glEnd();
                }
            }
        }
    }
}

void GridDataOpenGLWidget::setGridDataConfiguration(GridDataConfiguration *gridDataConfiguration) {
    this->gridDataConfiguration = gridDataConfiguration;
    update();
}

void GridDataOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    if (gridDataConfiguration == NULL || gridDataConfiguration->getMesh() == NULL) {
        return;
    }

    int x = event->x(), y = event->y();
    double ratioX = x / (double) this->width(), ratioY = y / (double) this->height();
    double mapWidth = right - left, mapHeight = top - bottom;
    Point realCoordinate(left + mapWidth * ratioX, top - mapHeight * ratioY);
    QSet<CellInfo*> cellInfoSet = gridDataConfiguration->queryCells(realCoordinate);

    if (!cellInfoSet.isEmpty()) {
        CellUpdateDialog *cellUpdateDialog = new CellUpdateDialog((QWidget*) this->parent(), cellInfoSet);

        int exitCode = cellUpdateDialog->exec();

        if (exitCode == QDialog::Accepted) {
            QList<CellInfo> cellInfoList = cellUpdateDialog->getCellInfoList();

            for (int i = 0; i < cellInfoList.count(); i++) {
                const CellInfo &cellInfo = cellInfoList.at(i);

                for (QSet<CellInfo*>::iterator it = cellInfoSet.begin(); it != cellInfoSet.end(); it++) {
                    if (cellInfo.getGridInformationType() == (*it)->getGridInformationType()) {
                        (*it)->setWeight(cellInfo.getWeight());
                        break;
                    }
                }
            }
        }
    }
}
