#ifndef WATER_QUALITY_BOUNDARY_CONDITION_H
#define WATER_QUALITY_BOUNDARY_CONDITION_H

#include "hydrodynamic_boundary_condition.h"

class WaterQualityBoundaryCondition : public BoundaryCondition {
private:
    HydrodynamicBoundaryCondition *hydrodynamicBoundaryCondition;
    QString variable;

public:
    WaterQualityBoundaryCondition();

    HydrodynamicBoundaryCondition* getHydrodynamicBoundaryCondition() const;
    void setHydrodynamicBoundaryCondition(HydrodynamicBoundaryCondition *hydrodynamicBoundaryCondition);

    SimulationDataType::WaterQualityBoundaryCondition toSimulationDataType() const;
};

#endif // WATER_QUALITY_BOUNDARY_CONDITION_H
