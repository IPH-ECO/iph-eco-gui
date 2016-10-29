#ifndef HYDRODYNAMIC_CONFIGURATION_H
#define HYDRODYNAMIC_CONFIGURATION_H

#include "simulation_data_type.h"
#include "grid_data_configuration.h"
#include "hydrodynamic_boundary_condition.h"
#include "hydrodynamic_process.h"

#include <QString>
#include <QList>
#include <QSet>

class GridDataConfiguration;
class WaterQualityConfiguration;

class HydrodynamicConfiguration {
private:
	uint id;
	QString name;
	QList<HydrodynamicParameter*> parameters;
    GridDataConfiguration *gridDataConfiguration;
	QList<HydrodynamicBoundaryCondition*> boundaryConditions;
    QSet<WaterQualityConfiguration*> waterQualityConfigurations;
public:
    HydrodynamicConfiguration();
    ~HydrodynamicConfiguration();

	uint getId() const;
	void setId(const uint &id);
    bool isPersisted() const;
	QString getName() const;
	void setName(const QString &name);
	GridDataConfiguration* getGridDataConfiguration() const;
	void setGridDataConfiguration(GridDataConfiguration *gridDataConfiguration);
	
    bool addBoundaryCondition(HydrodynamicBoundaryCondition *boundaryCondition);
	QList<HydrodynamicBoundaryCondition*> getBoundaryConditions() const;
    HydrodynamicBoundaryCondition* getBoundaryCondition(const QString &boundaryConditionName) const;
	void setBoundaryConditions(const QList<HydrodynamicBoundaryCondition*> &boundaryConditions);
    void removeBoundaryCondition(int i);
    void clearBoundaryConditions();

	bool addHydrodynamicParameter(HydrodynamicParameter *hydrodynamicParameter);
	QList<HydrodynamicParameter*> getParameters() const;
	HydrodynamicParameter* getParameter(const QString &name) const;
    QList<HydrodynamicParameter*> getRootParameters() const;
    QSet<WaterQualityConfiguration*> getWaterQualityConfigurations() const;
    void addWaterQualityConfiguration(WaterQualityConfiguration *configuration);
    void removeWaterQualityConfiguration(WaterQualityConfiguration *configuration);
    
    SimulationDataType::HydrodynamicConfiguration* toSimulationDataType() const;
};

#endif // HYDRODYNAMIC_CONFIGURATION_H