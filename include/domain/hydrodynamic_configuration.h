#ifndef HYDRODYNAMIC_CONFIGURATION_H
#define HYDRODYNAMIC_CONFIGURATION_H

#include "include/domain/grid_data_configuration.h"
#include "include/domain/boundary_condition.h"
#include "include/domain/hydrodynamic_process.h"

#include <QString>
#include <QList>

class HydrodynamicConfiguration {
private:
	uint id;
	QString name;
	QList<HydrodynamicParameter*> parameters;
    GridDataConfiguration *gridDataConfiguration;
	QList<BoundaryCondition*> boundaryConditions;
public:
    HydrodynamicConfiguration();
    ~HydrodynamicConfiguration();

	uint getId() const;
	void setId(uint id);
    bool isPersisted() const;
	QString getName() const;
	void setName(const QString &name);
	GridDataConfiguration* getGridDataConfiguration() const;
	void setGridDataConfiguration(GridDataConfiguration *gridDataConfiguration);
	
    bool addBoundaryCondition(BoundaryCondition *boundaryCondition);
	QList<BoundaryCondition*> getBoundaryConditions() const;
    BoundaryCondition* getBoundaryCondition(int i) const;
	void setBoundaryConditions(const QList<BoundaryCondition*> &boundaryConditions);
    void removeBoundaryCondition(int i);
    void clearBoundaryConditions();

	bool addHydrodynamicParameter(HydrodynamicParameter *hydrodynamicParameter);
	QList<HydrodynamicParameter*> getParameters() const;
	HydrodynamicParameter* getParameter(const QString &name) const;
    QList<HydrodynamicParameter*> getRootParameters() const;
};

#endif // HYDRODYNAMIC_CONFIGURATION_H