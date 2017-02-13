#ifndef WATER_QUALITY_BOUNDARY_CONDITION_H
#define WATER_QUALITY_BOUNDARY_CONDITION_H

#include "hydrodynamic_boundary_condition.h"

class HydrodynamicBoundaryCondition;
class WaterQualityConfiguration;

class WaterQualityBoundaryCondition : public BoundaryCondition {
private:
    WaterQualityConfiguration *waterQualityConfiguration;
    HydrodynamicBoundaryCondition *hydrodynamicBoundaryCondition;

public:
    WaterQualityBoundaryCondition();

    WaterQualityConfiguration* getWaterQualityConfiguration() const;
    void setWaterQualityConfiguration(WaterQualityConfiguration *waterQualityConfiguration);
    HydrodynamicBoundaryCondition* getHydrodynamicBoundaryCondition() const;
    void setHydrodynamicBoundaryCondition(HydrodynamicBoundaryCondition *hydrodynamicBoundaryCondition);
    
    SimulationDataType::BoundaryCondition toSimulationDataType(Mesh *mesh) const;
};

#endif // WATER_QUALITY_BOUNDARY_CONDITION_H
