#include "include/domain/grid_data_configuration.h"

#include "include/domain/unstructured_mesh.h"
#include "include/domain/structured_mesh.h"
#include "include/exceptions/grid_data_exception.h"

#include <vtkDoubleArray.h>
#include <vtkCellData.h>
#include <vtkTriangle.h>
#include <vtkRegularPolygonSource.h>
#include <vtkPointData.h>

GridDataConfiguration::GridDataConfiguration() : mesh(NULL), showMesh(true) {}

GridDataConfiguration::~GridDataConfiguration() {
    for (int i = 0; i < gridDataVector.size(); i++) {
        delete gridDataVector.at(i);
    }
}

QString GridDataConfiguration::getName() const {
    return this->name;
}

void GridDataConfiguration::setName(const QString &name) {
    this->name = name;
}

Mesh* GridDataConfiguration::getMesh() const {
    return mesh;
}

void GridDataConfiguration::setMesh(Mesh *mesh) {
    this->mesh = mesh;
}

bool GridDataConfiguration::getShowMesh() const {
    return showMesh;
}

void GridDataConfiguration::setShowMesh(const bool showMesh) {
    this->showMesh = showMesh;
}

QVector<GridData*>& GridDataConfiguration::getGridDataVector() {
    return gridDataVector;
}

bool GridDataConfiguration::addGridData(GridData *gridData) {
    if (this->gridDataVector.contains(gridData)) {
        return false;
    }

    this->processGridData(gridData);
    this->gridDataVector.push_back(gridData);
    return true;
}

void GridDataConfiguration::removeGridData(int i) {
    GridData *gridData = NULL;

    if (i < gridDataVector.count()) {
        gridData = gridDataVector.at(i);
        gridDataVector.remove(i);
    }

    delete gridData;
}

GridData* GridDataConfiguration::getGridData(int i) {
    if (i < gridDataVector.count()) {
        return gridDataVector.at(i);
    }

    return NULL;
}

// CellInfo* GridDataConfiguration::createCellInfoFromDataPoints(Point &centroid, GridData *gridData, QSet<GridDataPoint> &dataPoints) {
//     GridDataType gridDataType = gridData->getGridDataType();
//     CGAL::Circle_2<K> circle(centroid, gridData->getRadius());
//     QSet<GridDataPoint> tempDataPoints;

//     for (QSet<GridDataPoint>::const_iterator it = dataPoints.begin(); it != dataPoints.end(); it++) {
//         if (circle.bounded_side(it->getPoint()) == CGAL::ON_BOUNDED_SIDE) {
//             tempDataPoints.insert(*it);
//         }
//     }

//     double weight;

//     if (tempDataPoints.isEmpty()) {
//         weight = calculateNearestWeight(dataPoints, centroid);
//     } else {
//         weight = inverseOfDistance(gridData, tempDataPoints, centroid);
//     }

//     return new CellInfo(gridData, gridDataType, weight);
// }

void GridDataConfiguration::processGridData(GridData *gridData) {
    GridDataType gridDataType = gridData->getGridDataType();
    vtkPolyData *gridPolyData = gridData->getData();
    double *gridDataPointer = static_cast<double*>(gridPolyData->GetPoints()->GetData()->GetVoidPointer(0));
    vtkIdType gridNumberOfPoints = gridPolyData->GetPoints()->GetNumberOfPoints();
    vtkPolyData *meshPolyData = mesh->getGrid();
    vtkCellArray *polygons = meshPolyData->GetPolys();
    const char *gridDataInputTypeStr = gridData->getGridDataType().toString().toStdString().c_str();
    vtkSmartPointer<vtkDoubleArray> weightsArray = vtkDoubleArray::SafeDownCast(meshPolyData->GetCellData()->GetArray(gridDataInputTypeStr));

    if (weightsArray == NULL) {
        weightsArray = vtkSmartPointer<vtkDoubleArray>::New();
        weightsArray->SetNumberOfTuples(meshPolyData->GetNumberOfCells());
        weightsArray->SetNumberOfComponents(1);
        weightsArray->SetName(gridDataInputTypeStr);
    }

    if (mesh->instanceOf("UnstructuredMesh")) {
        for (vtkIdType i = 0; i < meshPolyData->GetNumberOfCells(); i++) {
            vtkTriangle *triangle = vtkTriangle::SafeDownCast(meshPolyData->GetCell(i));
            double centroid[3], bounds[3], normal[3] = { 0.0, 0.0, 1.0 };

            triangle->GetParametricCenter(centroid);
            gridPolyData->GetPoints()->GetBounds(bounds);

            if (gridData->getGridDataInputType() == GridDataInputType::POINT) {
                vtkSmartPointer<vtkRegularPolygonSource> circleSource = vtkSmartPointer<vtkRegularPolygonSource>::New();
                circleSource->SetNumberOfSides(50);
                circleSource->SetRadius(gridData->getRadius());
                circleSource->SetCenter(centroid);
                circleSource->Update();

                vtkPolyData *circlePolyData = circleSource->GetOutput();
                double *circleDataPointer = static_cast<double*>(circlePolyData->GetPoints()->GetData()->GetVoidPointer(0));
                double circleBounds[3];

                circlePolyData->GetPoints()->GetBounds(circleBounds);

                for (vtkIdType j = 0; j < gridNumberOfPoints; j++) {
                    double point[3];

                    gridPolyData->GetPoints()->GetPoint(j, point);

                    if (vtkPolygon::PointInPolygon(point, circlePolyData->GetPoints()->GetNumberOfPoints(), circleDataPointer, circleBounds, normal)) {

                    }
                }
            } else {
                double tuple[1] = { 0.0 };

                if (vtkPolygon::PointInPolygon(centroid, gridNumberOfPoints, gridDataPointer, bounds, normal)) {
                    double weight = vtkDoubleArray::SafeDownCast(gridPolyData->GetPointData()->GetScalars())->GetValue(0);
                    tuple[0] = weight;
                }
                weightsArray->SetTuple(i, tuple);
            }
        }
        meshPolyData->GetCellData()->AddArray(weightsArray);
    } else {
        StructuredMesh *structuredMesh = static_cast<StructuredMesh*>(mesh);
        // matrix<Quad*> &grid = structuredMesh->getGrid();

        // for (ulong i = 0; i < grid.size1(); i++) {
        //     for (ulong j = 0; j < grid.size2(); j++) {
        //         Quad *quad = grid(i, j);

        //         if (quad == NULL || quad->is_empty()) {
        //             continue;
        //         }

        //         Point centroid = CGAL::centroid((*quad)[0], (*quad)[1], (*quad)[2], (*quad)[3]);
        //         CellInfo *cellInfo = NULL;

        //         if (isInputTypePoint) {
        //             cellInfo = createCellInfoFromDataPoints(centroid, gridData, dataPoints);
        //         } else {
        //             cellInfo = createCellInfoFromDataPolygon(centroid, gridData, dataPolygon);
        //         }

        //         if (cellInfo != NULL) {
        //             quad->addCellInfo(cellInfo);
        //         }
        //     }
        // }
    }
}

// double GridDataConfiguration::inverseOfDistance(GridData *gridData, QSet<GridDataPoint> &dataPoints, Point &centroid) {
//     double numerator = 0.0, denominator = 0.0;

//     for (QSet<GridDataPoint>::const_iterator it = dataPoints.begin(); it != dataPoints.end(); it++) {
//         double distance = CGAL::squared_distance(centroid, it->getPoint());
//         numerator += it->getData() / qPow(distance, gridData->getExponent());
//         denominator += 1 / qPow(distance, gridData->getExponent());
//     }

//     return numerator / denominator;
// }

// double GridDataConfiguration::calculateNearestWeight(QSet<GridDataPoint> &dataPoints, Point &centroid) {
//     QSet<GridDataPoint>::const_iterator it = dataPoints.begin();
//     QSet<GridDataPoint>::const_iterator nearest = it;
//     double minimalDistance = CGAL::squared_distance(centroid, nearest->getPoint());

//     while (++it != dataPoints.end()) {
//         double distance = CGAL::squared_distance(centroid, it->getPoint());

//         if (minimalDistance > distance) {
//             nearest = it;
//             minimalDistance = distance;
//         }
//     }

//     return nearest->getData();
// }

// QSet<CellInfo*> GridDataConfiguration::queryCells(Point &point) {
//     if (dynamic_cast<UnstructuredMesh*>(mesh) != NULL) {
//         UnstructuredMesh *unstructuredMesh = static_cast<UnstructuredMesh*>(mesh);
//         const CDT *cdt = NULL; //unstructuredMesh->getCDT();

//         for (CDT::Finite_faces_iterator fit = cdt->finite_faces_begin(); fit != cdt->finite_faces_end(); ++fit) {
//             if (!fit->info().isInDomain()) {
//                 continue;
//             }

//             CGAL::Triangle_2<K> triangle = cdt->triangle(fit);

//             if (triangle.bounded_side(point) == CGAL::ON_BOUNDED_SIDE) {
//                 return fit->info().getCellInfoSet();
//             }
//         }
//     } else {
//         StructuredMesh *structuredMesh = static_cast<StructuredMesh*>(mesh);
        // matrix<Quad*> &grid = structuredMesh->getGrid();

        // for (ulong i = 0; i < grid.size1(); i++) {
        //     for (ulong j = 0; j < grid.size2(); j++) {
        //         Quad *quad = grid(i, j);

        //         if (quad == NULL || quad->is_empty()) {
        //             continue;
        //         }

        //         if (quad->bounded_side(point) == CGAL::ON_BOUNDED_SIDE) {
        //             return quad->getCellInfoSet();
        //         }
        //     }
        // }
    // }

    // return QSet<CellInfo*>();
// }
