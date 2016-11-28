#include <domain/water_quality_boundary_condition.h>

WaterQualityBoundaryCondition::WaterQualityBoundaryCondition() :
	hydrodynamicBoundaryCondition(nullptr)
{}

HydrodynamicBoundaryCondition* WaterQualityBoundaryCondition::getHydrodynamicBoundaryCondition() const {
	return hydrodynamicBoundaryCondition;
}

void WaterQualityBoundaryCondition::setHydrodynamicBoundaryCondition(HydrodynamicBoundaryCondition *hydrodynamicBoundaryCondition) {
	this->hydrodynamicBoundaryCondition = hydrodynamicBoundaryCondition;
}

SimulationDataType::BoundaryCondition WaterQualityBoundaryCondition::toSimulationDataType() const {
    SimulationDataType::BoundaryCondition boundaryCondition;
    QByteArray conditionType = this->name.toLocal8Bit();
    
    boundaryCondition.inputModule = (int) this->inputModule;
    strncpy(boundaryCondition.conditionType, conditionType.constData(), conditionType.size());
    boundaryCondition.conditionFunction = (int) this->function;
    boundaryCondition.constantValue = this->constantValue;
    boundaryCondition.timeSeriesListSize = this->timeSeriesList.size();
    boundaryCondition.timeSeriesList = new SimulationDataType::TimeSeries[boundaryCondition.timeSeriesListSize];
    
    for (vtkIdType i = 0; i < this->timeSeriesList.size(); i++) {
        boundaryCondition.timeSeriesList[i] = this->timeSeriesList[i]->toSimulationDataType();
    }
    
    boundaryCondition.verticalIntegratedOutflow = this->verticalIntegratedOutflow;
    boundaryCondition.minimumElevation = this->minimumElevation;
    boundaryCondition.maximumElevation = this->maximumElevation;
    
    return boundaryCondition;
}
