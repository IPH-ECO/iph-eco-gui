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
		structuredMesh.numberOfElements = numberOfCells;
        structuredMesh.resolution = static_cast<StructuredMesh*>(mesh)->getResolution();
        structuredMesh.xCoordinates = new double[numberOfCells];
		structuredMesh.yCoordinates = new double[numberOfCells];
        
        for (vtkIdType i = 0; i < cellCentersFilter->GetOutput()->GetNumberOfPoints(); i++) {
            double center[3];
            cellCentersFilter->GetOutput()->GetPoint(i, center);
            structuredMesh.xCoordinates[i] = center[0];
            structuredMesh.yCoordinates[i] = center[1];
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
        
        structuredMesh.northNeighbors = new vtkIdType[numberOfCells];
        structuredMesh.westNeighbors = new vtkIdType[numberOfCells];
        structuredMesh.southNeighbors = new vtkIdType[numberOfCells];
        structuredMesh.eastNeighbors = new vtkIdType[numberOfCells];
        
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
                        directionArray = structuredMesh.southNeighbors;
                    } else if (directionIndex == (vtkIdType) EdgeDirection::EAST) {
                        directionArray = structuredMesh.eastNeighbors;
                    } else if (directionIndex == (vtkIdType) EdgeDirection::NORTH) {
                        directionArray = structuredMesh.northNeighbors;
                    } else {
                        directionArray = structuredMesh.westNeighbors;
                    }
                    
                    directionArray[cellId] = boundaryCellIds.contains(neighborId) ? -2 : neighborId;
                    directionIndex++;
                }
            }
        }
	} else {
        SimulationModel::UnstructuredMesh unstructuredMesh;
        vtkIdType numberOfPoints = meshPolyData->GetNumberOfPoints();
        
        unstructuredMesh.numberOfPoints = numberOfPoints;
        unstructuredMesh.numberOfElements = meshPolyData->GetNumberOfCells();
        unstructuredMesh.xCoordinateIds = new double[numberOfPoints];
        unstructuredMesh.yCoordinateIds = new double[numberOfPoints];
        
        for (vtkIdType i = 0; i < numberOfPoints; i++) {
            double point[3];
            meshPolyData->GetPoints()->GetPoint(i, point);
            unstructuredMesh.xCoordinateIds[i] = point[0];
            unstructuredMesh.yCoordinateIds[i] = point[1];
        }
        
        unstructuredMesh.vertexIds = new vtkIdType*[numberOfCells];
        
        for (vtkIdType i = 0; i < numberOfCells; i++) {
            vtkSmartPointer<vtkIdList> vertices = vtkSmartPointer<vtkIdList>::New();
            meshPolyData->GetCellPoints(i, vertices);
            unstructuredMesh.vertexIds[i] = new vtkIdType[vertices->GetNumberOfIds()]; // 3
            
            for (vtkIdType j = 0; j < vertices->GetNumberOfIds(); j++) {
                unstructuredMesh.vertexIds[i][j] = vertices->GetId(j);
            }
        }
	}
    
    SimulationModel::GridData simulationGridData;
    simulationGridData.numberOfElements = numberOfCells;
    simulationGridData.bathymetry = new double[numberOfCells];
    simulationGridData.roughness = new double[numberOfCells];
    simulationGridData.hasWindReductionCoefficient = false;
    simulationGridData.hasWetland = false;
    simulationGridData.has50GrainSize = false;
    simulationGridData.hasOrganicMatterFraction = false;
    simulationGridData.hasQuotaOfImpermeableLayer = false;
    
    for (GridData *gridData : gridDataConfiguration->getGridDataVector()) {
        std::string arrayName = gridData->getName().toStdString();
        vtkSmartPointer<vtkDoubleArray> weights = vtkDoubleArray::SafeDownCast(meshPolyData->GetCellData()->GetArray(arrayName.c_str()));
        GridDataType gridDataType = gridData->getGridDataType();
        double *weightsArray = nullptr;
        bool *booleanField = nullptr;
        
        if (gridDataType == GridDataType::BATHYMETRY) {
            weightsArray = simulationGridData.bathymetry;
        } else if (gridDataType == GridDataType::ROUGHNESS) {
            weightsArray = simulationGridData.bathymetry;
        } else if (gridDataType == GridDataType::WIND_REDUCTION) {
            booleanField = &simulationGridData.hasWindReductionCoefficient;
            weightsArray = simulationGridData.windReductionCoefficient;
        } else if (gridDataType == GridDataType::WETLAND_AREA) {
            booleanField = &simulationGridData.hasWetland;
            weightsArray = simulationGridData.wetland;
        } else if (gridDataType == GridDataType::D50_GRAIN_SIZE) {
            booleanField = &simulationGridData.has50GrainSize;
            weightsArray = simulationGridData.d50GrainSize;
        } else if (gridDataType == GridDataType::FRACTION_OF_ORGANIC_MATTER) {
            booleanField = &simulationGridData.hasOrganicMatterFraction;
            weightsArray = simulationGridData.organicMatterFraction;
//        } else if (gridDataType == GridDataType::) {
//            booleanField = &simulationGridData.hasOrganicMatterFraction;
//            weightsArray = simulationGridData.organicMatterFraction;
        }
        
        if (!booleanField && gridDataType != GridDataType::BATHYMETRY && gridDataType != GridDataType::ROUGHNESS) {
            *&weightsArray = new double[numberOfCells];
            *booleanField = true;
        }
        
        for (vtkIdType i = 0; i < weights->GetNumberOfTuples(); i++) {
            weightsArray[i] = weights->GetTuple1(i);
        }
    }
}
