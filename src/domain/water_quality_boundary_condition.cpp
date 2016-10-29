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

QString WaterQualityBoundaryCondition::getVariable() const {
	return variable;
}

void WaterQualityBoundaryCondition::setVariable(const QString &variable) {
	this->variable = variable;
}

SimulationDataType::WaterQualityBoundaryCondition WaterQualityBoundaryCondition::toSimulationDataType() const {

}