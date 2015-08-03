#include "include/domain/hydrodynamic_configuration.h"

HydrodynamicConfiguration::HydrodynamicConfiguration() : id(0) {}

HydrodynamicConfiguration::~HydrodynamicConfiguration() {
    for (int i = 0; i < parameters.size(); i++) {
        delete parameters[i];
    }
    parameters.empty();
}

uint HydrodynamicConfiguration::getId() const {
	return id;
}

void HydrodynamicConfiguration::setId(uint id) {
	this->id = id;
}

bool HydrodynamicConfiguration::isPersisted() const {
	return id != 0;
}

QString HydrodynamicConfiguration::getName() const {
	return name;
}

void HydrodynamicConfiguration::setName(const QString &name) {
	this->name = name;
}

Mesh* HydrodynamicConfiguration::getMesh() const {
	return mesh;
}

void HydrodynamicConfiguration::setMesh(Mesh *mesh) {
	this->mesh = mesh;
}

bool HydrodynamicConfiguration::addBoundaryCondition(BoundaryCondition *boundaryCondition) {
	if (boundaryConditions.contains(boundaryCondition)) {
		return false;
	}

	boundaryConditions.append(boundaryCondition);

	return true;
}

QList<BoundaryCondition*> HydrodynamicConfiguration::getBoundaryConditions() const {
	return boundaryConditions;
}

void HydrodynamicConfiguration::setBoundaryConditions(const QList<BoundaryCondition*> &boundaryConditions) {
	this->boundaryConditions = boundaryConditions;
}

void HydrodynamicConfiguration::removeBoundaryCondition(int i) {
    this->boundaryConditions.removeAt(i);
}

bool HydrodynamicConfiguration::addHydrodynamicParameter(HydrodynamicParameter *hydrodynamicParameter) {
	if (parameters.contains(hydrodynamicParameter)) {
		return false;
	}

	parameters.append(hydrodynamicParameter);

	return true;
}

QList<HydrodynamicParameter*> HydrodynamicConfiguration::getParameters() const {
	return parameters;
}

HydrodynamicParameter* HydrodynamicConfiguration::getParameter(const QString &name) const {
	for (int i = 0; i < parameters.size(); i++) {
		if (parameters[i]->getName() == name) {
			return parameters[i];
		}
	}

	return nullptr;
}

QList<HydrodynamicParameter*> HydrodynamicConfiguration::getRootParameters() const {
    QList<HydrodynamicParameter*> rootParameters;
    
    for (int i = 0; i < parameters.size(); i++) {
        if (!parameters[i]->getParent()) {
            rootParameters.append(parameters[i]);
        }
    }
    
    qSort(rootParameters.begin(), rootParameters.end(), HydrodynamicParameter::sort);
    
    return rootParameters;
}

void HydrodynamicConfiguration::clearBoundaryConditions() {
	for (int i = 0; i < boundaryConditions.size(); i++) {
//        delete boundaryConditions.at(i);
    }
    boundaryConditions.clear();
}