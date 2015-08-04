#ifndef HYDRODYNAMIC_CONFIGURATION_H
#define HYDRODYNAMIC_CONFIGURATION_H

#include "include/domain/mesh.h"
#include "include/domain/boundary_condition.h"
#include "include/domain/hydrodynamic_process.h"

#include <QString>
#include <QList>

class HydrodynamicConfiguration {
private:
	uint id;
	QString name;
	QList<HydrodynamicParameter*> parameters;
    Mesh *mesh;
	QList<BoundaryCondition*> boundaryConditions;
public:
    HydrodynamicConfiguration();
    ~HydrodynamicConfiguration();

	uint getId() const;
	void setId(uint id);
	QString getName() const;
	void setName(const QString &name);
	Mesh* getMesh() const;
	void setMesh(Mesh *mesh);
	bool addBoundaryCondition(BoundaryCondition *boundaryCondition);
	QList<BoundaryCondition*> getBoundaryConditions() const;
    BoundaryCondition* getBoundaryCondition(int i) const;
	void setBoundaryConditions(const QList<BoundaryCondition*> &boundaryConditions);
    void removeBoundaryCondition(int i);

	bool addHydrodynamicParameter(HydrodynamicParameter *hydrodynamicParameter);
	QList<HydrodynamicParameter*> getParameters() const;
	HydrodynamicParameter* getParameter(const QString &name) const;
    QList<HydrodynamicParameter*> getRootParameters() const;
	bool isPersisted() const;

	void clearBoundaryConditions();
};

#endif // HYDRODYNAMIC_CONFIGURATION_H