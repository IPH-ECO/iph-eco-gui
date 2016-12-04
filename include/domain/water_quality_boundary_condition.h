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
    virtual bool instanceOf(const QString &typeName) const;

    SimulationDataType::BoundaryCondition toSimulationDataType() const;
};

#endif // WATER_QUALITY_BOUNDARY_CONDITION_H
