#include <domain/structured_mesh.h>

#include <domain/hydrodynamic_boundary_condition.h>

#include <vtkQuad.h>
#include <vtkIdList.h>
#include <vtkPoints.h>
#include <vtkPolygon.h>
#include <QApplication>
#include <vtkCellData.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>

StructuredMesh::StructuredMesh() : resolution(100) {}

double StructuredMesh::getResolution() const {
    return resolution;
}

void StructuredMesh::setResolution(double resolution) {
    this->resolution = resolution;
}

bool StructuredMesh::instanceOf(const QString &type) {
    return type.contains("StructuredMesh");
}

void StructuredMesh::generate() {
    ulong bounds[4];
    
    computeBounds(bounds);
    
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> quads = vtkSmartPointer<vtkCellArray>::New();
    int columns = (bounds[1] - bounds[0]) / this->resolution + 1; // xmax - xmin
    int rows = (bounds[3] - bounds[2]) / this->resolution + 1; // ymax - ymin
    double x = bounds[0];
    double y = bounds[2];
    int count = 0, currentStep = 0;
    QMap<std::pair<double, double>, vtkIdType> pointsMap;
    
    for (int i = 0; i < rows && !generationCanceled; i++) {
        for (int j = 0; j < columns && !generationCanceled; j++) {
            emit updateProgress(currentStep++);
            QApplication::processEvents();
            
            double quadCenter[3] = { x + this->resolution / 2.0, y + this->resolution / 2.0, 0.0 };
            bool includeQuad = this->pointInMesh(quadCenter);
            
            if (includeQuad) {
                vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
                std::pair<double, double> quadCoordinates[4] = {
                    std::pair<double, double>(x, y),
                    std::pair<double, double>(x + this->resolution, y),
                    std::pair<double, double>(x + this->resolution, y + this->resolution),
                    std::pair<double, double>(x, y + this->resolution)
                };
                
                quad->GetPointIds()->SetNumberOfIds(4);
                
                for (int k = 0; k < 4; k++) {
                    std::pair<double, double> point = quadCoordinates[k];
                    
                    if (!pointsMap.contains(point)) {
                        pointsMap.insert(point, count++);
                        points->InsertNextPoint(point.first, point.second, 0.0);
                    }
                    
                    quad->GetPointIds()->SetId(k, pointsMap.value(point));
                }
                
                quads->InsertNextCell(quad);
            }
            
            x += this->resolution;
        }
        x = bounds[0];
        y += this->resolution;
    }
    
    if (!generationCanceled) {
        meshPolyData = vtkSmartPointer<vtkPolyData>::New();
        meshPolyData->SetPoints(points);
        meshPolyData->SetPolys(quads);
        
        generateBoundaryPolyData();
    }
}

void StructuredMesh::computeBounds(ulong *points) {
    double *bounds = this->boundaryPolygon->getFilteredPolygon()->GetPoints()->GetBounds();

    points[0] = bounds[0] - fmod(bounds[0], this->resolution); // xmin
    points[1] = bounds[1] - fmod(bounds[1], this->resolution); // xmax
    points[2] = bounds[2] - fmod(bounds[2], this->resolution); // ymin
    points[3] = bounds[3] - fmod(bounds[3], this->resolution); // ymax
}

bool StructuredMesh::pointInMesh(double *point) {
    if (boundaryPolygon->pointInPolygon(point)) {
        QList<MeshPolygon*> islands = this->getIslands();

        for (QList<MeshPolygon*>::const_iterator it = islands.begin(); it != islands.end(); it++) {
            if ((*it)->pointInPolygon(point)) {
                return false;
            }
        }

        return true;
    }

    return false;
}

SimulationDataType::StructuredMesh* StructuredMesh::toSimulationDataType(const HydrodynamicConfiguration *hydrodynamicConfiguration) const {
    SimulationDataType::StructuredMesh *structuredMesh = new SimulationDataType::StructuredMesh();
    enum class EdgeDirection { SOUTH = 0, EAST, NORTH, WEST };
    
    HydrodynamicBoundaryCondition *waterFlowBoundaryCondition = nullptr;
    
    for (HydrodynamicBoundaryCondition *boundaryCondition : hydrodynamicConfiguration->getBoundaryConditions()) {
        if (boundaryCondition->getType() == BoundaryConditionType::WATER_FLOW) {
            waterFlowBoundaryCondition = boundaryCondition;
            break;
        }
    }
    
    QSet<vtkIdType> boundaryCellIds;
    
    if (waterFlowBoundaryCondition) {
        boundaryCellIds = this->getBoundaryCellIds(waterFlowBoundaryCondition->getVTKObjectIds());
    }
    
    vtkIdType numberOfCells = this->meshPolyData->GetNumberOfCells();
    
    structuredMesh->numberOfElements = numberOfCells;
    structuredMesh->resolution = this->resolution;
    structuredMesh->xCoordinates = new double[numberOfCells];
    structuredMesh->yCoordinates = new double[numberOfCells];
	structuredMesh->northNeighbors = new long long int[numberOfCells];
	structuredMesh->westNeighbors = new long long int[numberOfCells];
	structuredMesh->southNeighbors = new long long int[numberOfCells];
	structuredMesh->eastNeighbors = new long long int[numberOfCells];
    
    for (vtkIdType cellId = 0; cellId < numberOfCells; cellId++) {
        double cellBounds[6] = { 0 };
        
        this->meshPolyData->GetCell(cellId)->GetBounds(cellBounds);
        structuredMesh->xCoordinates[cellId] = cellBounds[0] + (cellBounds[1] - cellBounds[0]) / 2.0;
        structuredMesh->yCoordinates[cellId] = cellBounds[2] + (cellBounds[3] - cellBounds[2]) / 2.0;
        
        vtkSmartPointer<vtkIdList> cellNeighbors = vtkSmartPointer<vtkIdList>::New();
        vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
        vtkIdType directionIndex = 0;
        
        this->meshPolyData->GetCellPoints(cellId, cellPointIds);
        
        for (vtkIdType cellPointId = 0; cellPointId < cellPointIds->GetNumberOfIds(); cellPointId++) {
            vtkSmartPointer<vtkIdList> edge = vtkSmartPointer<vtkIdList>::New();
            
            edge->InsertNextId(cellPointIds->GetId(cellPointId));
            edge->InsertNextId(cellPointIds->GetId(cellPointId + 1 == cellPointIds->GetNumberOfIds() ? 0 : cellPointId + 1));
            this->meshPolyData->GetCellNeighbors(cellId, edge, cellNeighbors);
            
            if (cellNeighbors->GetNumberOfIds() == 0) {
                cellNeighbors->InsertNextId(-1);
            }
            
            for (vtkIdType i = 0; i < cellNeighbors->GetNumberOfIds(); i++) {
                long long int *directionArray = nullptr;
                vtkIdType neighborId = cellNeighbors->GetId(i);
                
                if (directionIndex == (vtkIdType) EdgeDirection::SOUTH) {
                    directionArray = structuredMesh->southNeighbors;
                } else if (directionIndex == (vtkIdType) EdgeDirection::EAST) {
                    directionArray = structuredMesh->eastNeighbors;
                } else if (directionIndex == (vtkIdType) EdgeDirection::NORTH) {
                    directionArray = structuredMesh->northNeighbors;
                } else if (directionIndex == (vtkIdType) EdgeDirection::WEST) {
                    directionArray = structuredMesh->westNeighbors;
                }
                
                directionArray[cellId] = boundaryCellIds.contains(neighborId) ? -2 : neighborId;
                directionIndex++;
            }
        }
    }
    
    structuredMesh->verticeIdsLength = this->meshPolyData->GetNumberOfCells() * 4;
    structuredMesh->verticeIds = new long long int[structuredMesh->verticeIdsLength];
    vtkIdType count = 0;
    
    for (vtkIdType i = 0; i < this->meshPolyData->GetNumberOfCells(); i++) {
        vtkSmartPointer<vtkIdList> vertices = vtkSmartPointer<vtkIdList>::New();

        this->meshPolyData->GetCellPoints(i, vertices);
        for (vtkIdType j = 0; j < vertices->GetNumberOfIds(); j++) {
            structuredMesh->verticeIds[count++] = vertices->GetId(j);
        }
    }
    
    return structuredMesh;
}