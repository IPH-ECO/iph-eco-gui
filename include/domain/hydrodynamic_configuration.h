#ifndef HYDRODYNAMIC_CONFIGURATION_H
#define HYDRODYNAMIC_CONFIGURATION_H

#include "include/domain/hydrodynamic_process.h"
#include "include/domain/mesh.h"

#include <QString>
#include <QList>

class HydrodynamicConfiguration {
private:
	uint id;
	QString name;
	QList<HydrodynamicParameter*> parameters;
    Mesh *mesh;
	QList<int> boundaryConditions;
public:
    HydrodynamicConfiguration();
    ~HydrodynamicConfiguration();

	uint getId() const;
	void setId(uint id);
	QString getName() const;
	void setName(const QString &name);
	Mesh* getMesh() const;
	void setMesh(Mesh *mesh);
	QList<int> getBoundaryConditions() const;
	void setBoundaryConditions(const QList<int> &boundaryConditions);

	bool addHydrodynamicParameter(HydrodynamicParameter *hydrodynamicParameter);
	QList<HydrodynamicParameter*> getParameters() const;
	HydrodynamicParameter* getParameter(const QString &name) const;
    QList<HydrodynamicParameter*> getRootParameters() const;
	bool isPersisted() const;

	void clearBoundaryConditions();
};

#endif // HYDRODYNAMIC_CONFIGURATION_H