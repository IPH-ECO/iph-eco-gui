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

SimulationDataType::WaterQualityBoundaryCondition WaterQualityBoundaryCondition::toSimulationDataType() const {

}
