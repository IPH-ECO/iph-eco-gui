#include "include/services/simulation_service.h"

#include "include/domain/structured_mesh.h"
#include "include/domain/simulation_model.h"

#include <vtkQuad.h>
#include <vtkIdList.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkCellCenters.h>

SimulationService* SimulationService::instance = nullptr;

SimulationService::SimulationService() {}

SimulationService* SimulationService::getInstance() {
	if (!instance) {
		instance = new SimulationService();
	}

	return instance;
}

void SimulationService::run(HydrodynamicConfiguration *hydrodynamicConfiguration) {
    GridDataConfiguration *gridDataConfiguration = hydrodynamicConfiguration->getGridDataConfiguration();
	Mesh *mesh = gridDataConfiguration->getMesh();
	vtkSmartPointer<vtkPolyData> meshPolyData = mesh->getMeshPolyData();
    vtkIdType numberOfCells = meshPolyData->GetNumberOfCells();
    
    if (mesh->instanceOf("StructuredMesh")) {
        vtkSmartPointer<vtkCellCenters> cellCentersFilter = vtkSmartPointer<vtkCellCenters>::New();
        cellCentersFilter->SetInputData(meshPolyData);
        cellCentersFilter->Update();
        
		SimulationModel::StructuredMesh structuredMesh;
		structuredMesh.nElem = numberOfCells;
        structuredMesh.dx = static_cast<StructuredMesh*>(mesh)->getResolution();
        structuredMesh.CoordX = new double[numberOfCells];
		structuredMesh.CoordY = new double[numberOfCells];
        
        for (vtkIdType i = 0; i < cellCentersFilter->GetOutput()->GetNumberOfPoints(); i++) {
            double center[3];
            cellCentersFilter->GetOutput()->GetPoint(i, center);
            structuredMesh.CoordX[i] = center[0];
            structuredMesh.CoordY[i] = center[1];
        }
        
        BoundaryCondition *waterFlowBoundaryCondition = nullptr;
        
        for (BoundaryCondition *boundaryCondition : hydrodynamicConfiguration->getBoundaryConditions()) {
            if (boundaryCondition->getType() == BoundaryConditionType::WATER_FLOW) {
                waterFlowBoundaryCondition = boundaryCondition;
                break;
            }
        }
        
        QSet<vtkIdType> boundaryCellIds;
        
        if (waterFlowBoundaryCondition) {
            boundaryCellIds = mesh->getBoundaryCellIds(waterFlowBoundaryCondition->getVTKObjectIds());
        }
        
        structuredMesh.VIZN = new vtkIdType[numberOfCells];
        structuredMesh.VIZO = new vtkIdType[numberOfCells];
        structuredMesh.VIZS = new vtkIdType[numberOfCells];
        structuredMesh.VIZL = new vtkIdType[numberOfCells];
        
        for (vtkIdType cellId = 0; cellId < numberOfCells; cellId++) {
            vtkSmartPointer<vtkIdList> cellNeighbors = vtkSmartPointer<vtkIdList>::New();
            vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
            vtkIdType directionIndex = 0;
            
            meshPolyData->GetCellPoints(cellId, cellPointIds);
            
            for (vtkIdType cellPointId = 0; cellPointId < cellPointIds->GetNumberOfIds(); cellPointId++) {
                vtkSmartPointer<vtkIdList> edge = vtkSmartPointer<vtkIdList>::New();
                
                edge->InsertNextId(cellPointIds->GetId(cellPointId));
                edge->InsertNextId(cellPointIds->GetId(cellPointId + 1 == cellPointIds->GetNumberOfIds() ? 0 : cellPointId + 1));
                meshPolyData->GetCellNeighbors(cellId, edge, cellNeighbors);
                
                if (cellNeighbors->GetNumberOfIds() == 0) {
                    cellNeighbors->InsertNextId(-1);
                }
                
                for (vtkIdType i = 0; i < cellNeighbors->GetNumberOfIds(); i++) {
                    vtkIdType *directionArray = nullptr;
                    vtkIdType neighborId = cellNeighbors->GetId(i);
                    
                    if (directionIndex == (vtkIdType) EdgeDirection::SOUTH) {
                        directionArray = structuredMesh.VIZS;
                    } else if (directionIndex == (vtkIdType) EdgeDirection::EAST) {
                        directionArray = structuredMesh.VIZL;
                    } else if (directionIndex == (vtkIdType) EdgeDirection::NORTH) {
                        directionArray = structuredMesh.VIZN;
                    } else {
                        directionArray = structuredMesh.VIZO;
                    }
                    
                    directionArray[cellId] = boundaryCellIds.contains(neighborId) ? -2 : neighborId;
                    directionIndex++;
                }
            }
        }
	} else {
        SimulationModel::UnstructuredMesh unstructuredMesh;
        vtkIdType numberOfPoints = meshPolyData->GetNumberOfPoints();
        
        unstructuredMesh.nPoint = numberOfPoints;
        unstructuredMesh.nElem = meshPolyData->GetNumberOfCells();
        unstructuredMesh.CoordX_point = new double[numberOfPoints];
        unstructuredMesh.CoordY_point = new double[numberOfPoints];
        
        for (vtkIdType i = 0; i < numberOfPoints; i++) {
            double point[3];
            meshPolyData->GetPoints()->GetPoint(i, point);
            unstructuredMesh.CoordX_point[i] = point[0];
            unstructuredMesh.CoordY_point[i] = point[1];
        }
        
        unstructuredMesh.Connect = new vtkIdType*[numberOfCells];
        
        for (vtkIdType i = 0; i < numberOfCells; i++) {
            vtkSmartPointer<vtkIdList> vertices = vtkSmartPointer<vtkIdList>::New();
            meshPolyData->GetCellPoints(i, vertices);
            unstructuredMesh.Connect[i] = new vtkIdType[vertices->GetNumberOfIds()]; // 3
            
            for (vtkIdType j = 0; j < vertices->GetNumberOfIds(); j++) {
                unstructuredMesh.Connect[i][j] = vertices->GetId(j);
            }
        }
	}
    
    SimulationModel::GridData simulationGridData;
    simulationGridData.nElem = numberOfCells;
    simulationGridData.zBatim = new double[numberOfCells];
    simulationGridData.Rug = new double[numberOfCells];
    
    for (GridData *gridData : gridDataConfiguration->getGridDataVector()) {
        std::string arrayName = gridData->getName().toStdString();
        vtkSmartPointer<vtkDoubleArray> weights = vtkDoubleArray::SafeDownCast(meshPolyData->GetCellData()->GetArray(arrayName.c_str()));
        double *weightsArray = nullptr;
        
        if (gridData->getGridDataType() == GridDataType::BATHYMETRY) {
            weightsArray = simulationGridData.zBatim;
        } else if (gridData->getGridDataType() == GridDataType::ROUGHNESS) {
            weightsArray = simulationGridData.Rug;
        }
        
        for (vtkIdType i = 0; i < weights->GetNumberOfTuples(); i++) {
            weightsArray[i] = weights->GetTuple1(i);
        }
    }
}
