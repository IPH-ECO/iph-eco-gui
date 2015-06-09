#include "include/domain/grid_data_configuration.h"

#include "include/domain/unstructured_mesh.h"
#include "include/domain/structured_mesh.h"
#include "include/exceptions/grid_data_exception.h"

#include <vtkDoubleArray.h>
#include <vtkCellData.h>
#include <vtkTriangle.h>
#include <vtkQuad.h>
#include <vtkRegularPolygonSource.h>
#include <vtkPointData.h>

GridDataConfiguration::GridDataConfiguration() : mesh(NULL), interpolationCanceled(false), showMesh(true) {}

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

QVector<GridData*> GridDataConfiguration::getGridDataVector() {
    return gridDataVector;
}

bool GridDataConfiguration::addGridData(GridData *gridData) {
    gridData->build();
    this->interpolate(gridData);
    
    if (this->gridDataVector.contains(gridData)) {
        return false;
    }

    this->gridDataVector.push_back(gridData);

    return true;
}

void GridDataConfiguration::removeGridData(GridData *gridData) {
    gridDataVector.removeOne(gridData);
    delete gridData;
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

void GridDataConfiguration::interpolate(GridData *gridData) {
    GridDataType gridDataType = gridData->getGridDataType();
    vtkPolyData *gridPolyData = gridData->getData();
    vtkDoubleArray *gridDataScalars = vtkDoubleArray::SafeDownCast(gridPolyData->GetPointData()->GetScalars());
    double *gridDataPointer = static_cast<double*>(gridPolyData->GetPoints()->GetData()->GetVoidPointer(0));
    vtkIdType gridNumberOfPoints = gridPolyData->GetPoints()->GetNumberOfPoints();
    vtkPolyData *meshPolyData = mesh->getGrid();
    const char *gridDataInputTypeStr = gridDataType.toString().toStdString().c_str();
    vtkSmartPointer<vtkDoubleArray> weightsArray = vtkDoubleArray::SafeDownCast(meshPolyData->GetCellData()->GetArray(gridDataInputTypeStr));

    if (weightsArray == NULL) {
        weightsArray = vtkSmartPointer<vtkDoubleArray>::New();
        weightsArray->SetNumberOfTuples(meshPolyData->GetNumberOfCells());
        weightsArray->SetNumberOfComponents(1);
        weightsArray->SetName(gridDataInputTypeStr);
    }
    
    double cellCenter[3], gridDataBounds[6], normal[3] = { 0.0, 0.0, 1.0 };
    
    gridPolyData->GetPoints()->GetBounds(gridDataBounds);

    for (vtkIdType i = 0; i < meshPolyData->GetNumberOfCells() && !interpolationCanceled; i++) {
        double tuple[1] = { 0.0 };
        
        if (mesh->instanceOf("UnstructuredMesh")) {
            vtkTriangle *triangle = vtkTriangle::SafeDownCast(meshPolyData->GetCell(i));
            double *a = triangle->GetPoints()->GetPoint(0);
            double *b = triangle->GetPoints()->GetPoint(1);
            double *c = triangle->GetPoints()->GetPoint(2);

            vtkTriangle::TriangleCenter(a, b, c, cellCenter);
        } else {
            vtkQuad *quad = vtkQuad::SafeDownCast(meshPolyData->GetCell(i));
            double quadBounds[6];
            
            quad->GetBounds(quadBounds);
            cellCenter[0] = quadBounds[0] + (quadBounds[1] - quadBounds[0]) / 2.0;
            cellCenter[1] = quadBounds[2] + (quadBounds[3] - quadBounds[2]) / 2.0;
            cellCenter[2] = 0.0;
        }
        
        if (gridData->getGridDataInputType() == GridDataInputType::POINT) {
            vtkSmartPointer<vtkRegularPolygonSource> circleSource = vtkSmartPointer<vtkRegularPolygonSource>::New();
            circleSource->SetNumberOfSides(50);
            circleSource->SetRadius(gridData->getRadius());
            circleSource->SetCenter(cellCenter);
            circleSource->Update();

            vtkPolyData *circlePolyData = circleSource->GetOutput();
            double *circleDataPointer = static_cast<double*>(circlePolyData->GetPoints()->GetData()->GetVoidPointer(0));
            vtkIdType circleNumberOfPoints = circlePolyData->GetPoints()->GetNumberOfPoints();
            double circleBounds[6];

            circlePolyData->GetPoints()->GetBounds(circleBounds);
            
            vtkSmartPointer<vtkPoints> inscribedPoints = vtkSmartPointer<vtkPoints>::New();
            
            for (vtkIdType j = 0; j < gridNumberOfPoints && !interpolationCanceled; j++) {
                double point[3];

                gridPolyData->GetPoints()->GetPoint(j, point);
                
                if (vtkPolygon::PointInPolygon(point, circleNumberOfPoints, circleDataPointer, circleBounds, normal)) {
                    inscribedPoints->InsertNextPoint(point);
                    inscribedPoints->GetData()->InsertNextTuple1(gridDataScalars->GetValue(j));
                }
            }
            
            if (inscribedPoints->GetNumberOfPoints() == 0) {
                tuple[0] = calculateNearestWeight(gridPolyData, cellCenter);
            } else {
                tuple[0] = inverseOfDistance(inscribedPoints, cellCenter, gridData->getExponent());
            }
        } else {
            if (vtkPolygon::PointInPolygon(cellCenter, gridNumberOfPoints, gridDataPointer, gridDataBounds, normal)) {
                tuple[0] = gridDataScalars->GetValue(0);
            }
        }
        
        weightsArray->SetTuple(i, tuple);
        emit updateProgress(i);
    }
    
    if (!interpolationCanceled) {
        meshPolyData->GetCellData()->AddArray(weightsArray);
        meshPolyData->GetCellData()->SetScalars(weightsArray);
    }
}

double GridDataConfiguration::inverseOfDistance(vtkPoints *inscribedPoints, double *cellCenter, const double &exponent) {
    double numerator = 0.0, denominator = 0.0;

    for (vtkIdType i = 0; i < inscribedPoints->GetNumberOfPoints(); i++) {
        double point[3];
        
        inscribedPoints->GetPoint(i, point);
        
        double distance = sqrt(pow(point[0] - cellCenter[0], 2.0) + pow(point[1] - cellCenter[1], 2.0));
        double distanceExponent = pow(distance, exponent);
        numerator += inscribedPoints->GetData()->GetTuple1(i) / distanceExponent;
        denominator += 1.0 / distanceExponent;
    }

    return numerator / denominator;
}

double GridDataConfiguration::calculateNearestWeight(vtkPolyData *gridPolyData, double *cellCenter) {
    vtkIdType nearestId = 0;
    double point[3];
    
    gridPolyData->GetPoints()->GetPoint(nearestId, point);
    
    double minimalDistance = sqrt(pow(point[0] - cellCenter[0], 2.0) + pow(point[1] - cellCenter[1], 2.0));
    
    for (vtkIdType i = 1; i < gridPolyData->GetNumberOfPoints(); i++) {
        gridPolyData->GetPoints()->GetPoint(i, point);
        
        double distance = sqrt(pow(point[0] - cellCenter[0], 2.0) + pow(point[1] - cellCenter[1], 2.0));

        if (minimalDistance > distance) {
            nearestId = i;
            minimalDistance = distance;
        }
    }

    return gridPolyData->GetPointData()->GetScalars()->GetTuple1(nearestId);
}

void GridDataConfiguration::cancelInterpolation(bool value) {
    this->interpolationCanceled = value;
}

bool GridDataConfiguration::interpolationWasCanceled() const {
    return interpolationCanceled;
}