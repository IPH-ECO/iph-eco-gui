#include <domain/hydrodynamic_configuration.h>

HydrodynamicConfiguration::HydrodynamicConfiguration() : id(0) {}

HydrodynamicConfiguration::~HydrodynamicConfiguration() {
    for (int i = 0; i < parameters.size(); i++) {
        delete parameters[i];
    }
    
    for (BoundaryCondition *boundaryCondition : boundaryConditions) {
        delete boundaryCondition;
    }
}

uint HydrodynamicConfiguration::getId() const {
	return id;
}

void HydrodynamicConfiguration::setId(const uint &id) {
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

GridDataConfiguration* HydrodynamicConfiguration::getGridDataConfiguration() const {
	return gridDataConfiguration;
}

void HydrodynamicConfiguration::setGridDataConfiguration(GridDataConfiguration *gridDataConfiguration) {
	this->gridDataConfiguration = gridDataConfiguration;
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

BoundaryCondition* HydrodynamicConfiguration::getBoundaryCondition(int i) const {
    if (i >= boundaryConditions.size()) {
        return nullptr;
    }
    
    return boundaryConditions[i];
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
	for (HydrodynamicParameter *parameter : parameters) {
		if (parameter->getName() == name) {
			return parameter;
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
        delete boundaryConditions[i];
    }
    boundaryConditions.clear();
}

SimulationDataType::HydrodynamicConfiguration* HydrodynamicConfiguration::toSimulationDataType() const {
    SimulationDataType::HydrodynamicConfiguration *configuration = new SimulationDataType::HydrodynamicConfiguration();
    int i = 0;
    
    configuration->parameters = new SimulationDataType::HydrodynamicParameter[this->parameters.size()];
    configuration->numberOfParameters = this->parameters.size();
    
    for (HydrodynamicParameter *parameter : this->parameters) {
        QByteArray name = parameter->getName().toLocal8Bit();
        
        configuration->parameters[i].length = (int) name.size();
        configuration->parameters[i].name = new char[name.size()];
        strncpy(configuration->parameters[i].name, name.constData(), name.size());
        configuration->parameters[i].value = parameter->getValue();
        i++;
    }
    
    configuration->boundaryConditions = new SimulationDataType::BoundaryCondition[this->boundaryConditions.size()];
    configuration->numberOfBoundaryConditions = this->boundaryConditions.size();
    i = 0;
    
    for (BoundaryCondition *boundaryCondition : this->boundaryConditions) {
        configuration->boundaryConditions[i++] = boundaryCondition->toSimulationDataType(this->gridDataConfiguration->getMesh());
    }
    
    configuration->gridDataConfiguration = this->gridDataConfiguration->toSimulationDataType(this);
    
    return configuration;
}