#include <domain/water_quality_boundary_condition.h>

WaterQualityBoundaryCondition::WaterQualityBoundaryCondition() :
	hydrodynamicBoundaryCondition(nullptr)
{}

WaterQualityConfiguration* WaterQualityBoundaryCondition::getWaterQualityConfiguration() const {
    return waterQualityConfiguration;
}

void WaterQualityBoundaryCondition::setWaterQualityConfiguration(WaterQualityConfiguration *waterQualityConfiguration) {
    this->waterQualityConfiguration = waterQualityConfiguration;
}

HydrodynamicBoundaryCondition* WaterQualityBoundaryCondition::getHydrodynamicBoundaryCondition() const {
	return hydrodynamicBoundaryCondition;
}

void WaterQualityBoundaryCondition::setHydrodynamicBoundaryCondition(HydrodynamicBoundaryCondition *hydrodynamicBoundaryCondition) {
	this->hydrodynamicBoundaryCondition = hydrodynamicBoundaryCondition;
    this->hydrodynamicBoundaryCondition->addWaterQualityBoundaryCondition(this);
}

SimulationDataType::BoundaryCondition WaterQualityBoundaryCondition::toSimulationDataType(Mesh *mesh) const {
    vtkSmartPointer<vtkPolyData> meshPolyData = mesh->getMeshPolyData();
    SimulationDataType::BoundaryCondition boundaryCondition;
    QByteArray conditionType = this->name.toLocal8Bit();
    
    strncpy(boundaryCondition.conditionType, conditionType.constData(), conditionType.size());
    boundaryCondition.verticallyIntegrated = this->verticallyIntegrated;
    
    if (this->verticallyIntegrated) {
        boundaryCondition.rangesSize = 0;
        boundaryCondition.conditionFunction = (int) this->function;
        boundaryCondition.constantValue = this->constantValue;
        boundaryCondition.timeSeriesListSize = this->timeSeriesList.size();
        boundaryCondition.timeSeriesList = new SimulationDataType::TimeSeries[boundaryCondition.timeSeriesListSize];
        
        for (int i = 0; i < this->timeSeriesList.size(); i++) {
            boundaryCondition.timeSeriesList[i] = this->timeSeriesList[i]->toSimulationDataType();
        }
    } else {
        int i = 0;
        
        boundaryCondition.rangesSize = this->nonVerticallyIntegratedRanges.size();
        boundaryCondition.ranges = new SimulationDataType::NonVerticallyIntegratedRange[boundaryCondition.rangesSize];
        
        for (NonVerticallyIntegratedRange *range : nonVerticallyIntegratedRanges) {
            boundaryCondition.ranges[i] = range->toSimulationDataType();
            i++;
        }
    }
    
    // TODO: code duplicated. Create a method in HydrodynamicBoundaryCondition class.
    
    int cellCount = 0;
    
    boundaryCondition.cellsLength = this->hydrodynamicBoundaryCondition->getObjectIds().size();
    boundaryCondition.cells = new SimulationDataType::HydrodynamicBoundaryConditionCell[boundaryCondition.cellsLength];
    
    if (this->hydrodynamicBoundaryCondition->getType() == BoundaryConditionType::WATER_LEVEL) {
        conditionType = "waterLevel";
        
        for (vtkIdType objectId : this->hydrodynamicBoundaryCondition->getObjectIds()) {
            boundaryCondition.cells[cellCount] = SimulationDataType::HydrodynamicBoundaryConditionCell();
            boundaryCondition.cells[cellCount].cellId = objectId;
            cellCount++;
        }
    } else {
        vtkSmartPointer<vtkPolyData> boundaryPolyData = mesh->getBoundaryPolyData();
        
        for (vtkIdType objectId : hydrodynamicBoundaryCondition->getObjectIds()) {
            vtkSmartPointer<vtkCell> edge = boundaryPolyData->GetCell(objectId);
            double edgeA[3], edgeB[3];
            
            edge->GetPoints()->GetPoint(0, edgeA);
            edge->GetPoints()->GetPoint(1, edgeB);
            
            vtkIdType meshPointAId = meshPolyData->FindPoint(edgeA);
            vtkIdType meshPointBId = meshPolyData->FindPoint(edgeB);
            vtkSmartPointer<vtkIdList> cellsPointA = vtkSmartPointer<vtkIdList>::New();
            vtkSmartPointer<vtkIdList> cellsPointB = vtkSmartPointer<vtkIdList>::New();
            
            meshPolyData->GetPointCells(meshPointAId, cellsPointA);
            meshPolyData->GetPointCells(meshPointBId, cellsPointB);
            
            vtkIdType foundCellId = -1;
            
            for (vtkIdType i = 0; i < cellsPointA->GetNumberOfIds() && foundCellId == -1; i++) {
                for (vtkIdType j = 0; j < cellsPointB->GetNumberOfIds() && foundCellId == -1; j++) {
                    if (cellsPointA->GetId(i) == cellsPointB->GetId(j)) {
                        foundCellId = i;
                    }
                }
            }
            
            boundaryCondition.cells[cellCount] = SimulationDataType::HydrodynamicBoundaryConditionCell();
            boundaryCondition.cells[cellCount].cellId = cellsPointA->GetId(foundCellId);
            boundaryCondition.cells[cellCount].verticeIds[0] = meshPolyData->FindPoint(edgeA);
            boundaryCondition.cells[cellCount].verticeIds[1] = meshPolyData->FindPoint(edgeB);
            
            cellCount++;
        }
        
        if (this->hydrodynamicBoundaryCondition->getType() == BoundaryConditionType::NORMAL_DEPTH) {
            conditionType = "normalDepth";
        }
    }
    
    return boundaryCondition;
}
