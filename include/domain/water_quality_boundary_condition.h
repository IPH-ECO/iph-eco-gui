#ifndef WATER_QUALITY_BOUNDARY_CONDITION_H
#define WATER_QUALITY_BOUNDARY_CONDITION_H

#include "hydrodynamic_boundary_condition.h"

class WaterQualityBoundaryCondition : public BoundaryCondition {
private:
    HydrodynamicBoundaryCondition *hydrodynamicBoundaryCondition;

public:
    WaterQualityBoundaryCondition();

    HydrodynamicBoundaryCondition* getHydrodynamicBoundaryCondition() const;
    void setHydrodynamicBoundaryCondition(HydrodynamicBoundaryCondition *hydrodynamicBoundaryCondition);
    
    SimulationDataType::BoundaryCondition toSimulationDataType(Mesh *mesh) const;
};

#endif // WATER_QUALITY_BOUNDARY_CONDITION_H
