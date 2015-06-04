#include "include/ui/grid_data_opengl_widget.h"

#include <QList>
#include <boost/numeric/ublas/matrix.hpp>

#include "include/ui/cell_update_dialog.h"
#include "include/utility/cgal_definitions.h"
#include "include/domain/structured_mesh.h"
#include "include/domain/unstructured_mesh.h"
#include "include/utility/opengl_util.h"
#include "include/domain/grid_information_type.h"

using boost::numeric::ublas::matrix;

GridDataOpenGLWidget::GridDataOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent), gridDataConfiguration(NULL), left(0), right(0), bottom(0), top(0)
{
    this->_parent = static_cast<GridDataDialog*>(parent);
    setMouseTracking(true);
}

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

//     left = boundaryPolygon->left_vertex()->x();
//     right = boundaryPolygon->right_vertex()->x();
//     bottom = boundaryPolygon->bottom_vertex()->y();
//     top = boundaryPolygon->top_vertex()->y();

//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     glOrtho(left - 50, right + 50, bottom - 50, top + 50, -1.0, 1.0);
//     glMatrixMode(GL_MODELVIEW);

//     glColor3f(0.0, 0.0, 0.0);

//     if (gridDataConfiguration->getShowMesh()) {
//         if (dynamic_cast<UnstructuredMesh*>(mesh) == NULL) {
//             StructuredMesh *structuredMesh = static_cast<StructuredMesh*>(mesh);
//             matrix<Quad*> &grid = structuredMesh->getGrid();

//             for (ulong i = 0; i < grid.size1(); i++) {
//                 for (ulong j = 0; j < grid.size2(); j++) {
//                     Quad *quad = grid(i, j);

//                     if (quad == NULL || quad->is_empty()) {
//                         continue;
//                     }

//                     QSet<CellInfo*> cells = quad->getCellInfoSet();
//                     QVector<double> color;

//                     for (QSet<CellInfo*>::const_iterator it = cells.begin(); it != cells.end(); it++) {
//                         if ((*it)->getGridInformationType().getValue() == GridInformationType::BATHYMETRY) {
//                             color = OpenGLUtil::convertToRGB(*it);
//                             glColor3d(color[0], color[1], color[2]);
//                             break;
//                         }
//                     }

//                     glBegin(GL_QUADS);
//                     glVertex2d((*quad)[0].x(), (*quad)[0].y());
//                     glVertex2d((*quad)[1].x(), (*quad)[1].y());
//                     glVertex2d((*quad)[2].x(), (*quad)[2].y());
//                     glVertex2d((*quad)[3].x(), (*quad)[3].y());
//                     glEnd();
//                 }
//             }
//         } else {
//             UnstructuredMesh *unstructuredMesh = static_cast<UnstructuredMesh*>(mesh);
//             const CDT *cdt = unstructuredMesh->getCDT();

//             glBegin(GL_TRIANGLES);
//             for (CDT::Finite_faces_iterator fit = cdt->finite_faces_begin(); fit != cdt->finite_faces_end(); ++fit) {
//                 if (!fit->info().isInDomain()) {
//                     continue;
//                 }

//                 QSet<CellInfo*> cells = fit->info().getCellInfoSet();
//                 QVector<double> color;

//                 for (QSet<CellInfo*>::const_iterator it = cells.begin(); it != cells.end(); it++) {
//                     if ((*it)->getGridInformationType().getValue() == GridInformationType::BATHYMETRY) {
//                         color = OpenGLUtil::convertToRGB(*it);
//                         glColor3d(color[0], color[1], color[2]);
//                         break;
//                     }
//                 }

//                 CGAL::Triangle_2<K> triangle = cdt->triangle(fit);

//                 glVertex2d(triangle[0].x(), triangle[0].y());
//                 glVertex2d(triangle[1].x(), triangle[1].y());
//                 glVertex2d(triangle[2].x(), triangle[2].y());
//             }
//             glEnd();
//         }

//         QVector<GridData*> &gridDataVector = gridDataConfiguration->getGridDataVector();

//         for (int i = 0; i < gridDataVector.count(); i++) {
//             GridData *gridData = gridDataVector.at(i);

// //            if (gridData->getShow()) {
//             if (false) {
//                 if (gridData->getGridInputType() == GridData::POINT) {
//                     QSet<GridDataPoint> &dataPoints = gridData->getGridDataPoints();

//                     //TODO: Refactor loop
//                     glBegin(GL_POINTS);
//                     for (QSet<GridDataPoint>::const_iterator it = dataPoints.begin(); it != dataPoints.end(); it++) {
//                         glVertex2f(it->getPoint().x(), it->getPoint().y());
//                     }
//                     glEnd();
//                 } else {
//                     GridDataPolygon &dataPolygon = gridData->getGridDataPolygon();

//                     glBegin(GL_LINE_LOOP);
//                     for (ulong j = 0; j < dataPolygon.size(); j++) {
//                         Point point = dataPolygon[j];
//                         glVertex2f(point.x(), point.y());
//                     }
//                     glEnd();
//                 }
//             }
//         }
//     }
}

void GridDataOpenGLWidget::setGridDataConfiguration(GridDataConfiguration *gridDataConfiguration) {
    this->gridDataConfiguration = gridDataConfiguration;
    update();
}

void GridDataOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    Point realCoordinate = OpenGLUtil::mapCoordinate(this, event, left, right, top, bottom);

    _parent->setRealCoordinate(realCoordinate);
}

void GridDataOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    if (gridDataConfiguration == NULL || gridDataConfiguration->getMesh() == NULL) {
        return;
    }

    Point realCoordinate = OpenGLUtil::mapCoordinate(this, event, left, right, top, bottom);
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
