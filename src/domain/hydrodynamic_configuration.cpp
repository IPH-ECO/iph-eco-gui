#include <domain/hydrodynamic_configuration.h>

#include <domain/water_quality_configuration.h>

HydrodynamicConfiguration::HydrodynamicConfiguration() : id(0) {}

HydrodynamicConfiguration::~HydrodynamicConfiguration() {
    for (int i = 0; i < parameters.size(); i++) {
        delete parameters[i];
    }
    
    for (HydrodynamicBoundaryCondition *boundaryCondition : boundaryConditions) {
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

bool HydrodynamicConfiguration::addBoundaryCondition(HydrodynamicBoundaryCondition *boundaryCondition) {
	if (boundaryConditions.contains(boundaryCondition)) {
		return false;
	}

	boundaryConditions.append(boundaryCondition);

	return true;
}

QList<HydrodynamicBoundaryCondition*> HydrodynamicConfiguration::getBoundaryConditions() const {
	return boundaryConditions;
}

HydrodynamicBoundaryCondition* HydrodynamicConfiguration::getBoundaryCondition(const QString &boundaryConditionName) const {
    for (HydrodynamicBoundaryCondition *boundaryCondition : boundaryConditions) {
        if (boundaryCondition->getName() == boundaryConditionName) {
            return boundaryCondition;
        }
    }
    
    return nullptr;
}

void HydrodynamicConfiguration::setBoundaryConditions(const QList<HydrodynamicBoundaryCondition*> &boundaryConditions) {
	this->boundaryConditions = boundaryConditions;
}

void HydrodynamicConfiguration::removeBoundaryCondition(int i) {
    HydrodynamicBoundaryCondition *boundaryCondition = this->boundaryConditions.at(i);
    QSet<WaterQualityBoundaryCondition*> waterQualityBoundaryConditions = boundaryCondition->getWaterQualityBoundaryConditions();
    
    for (WaterQualityBoundaryCondition *waterQualityBoundaryCondition : waterQualityBoundaryConditions) {
        WaterQualityConfiguration *waterQualityConfiguration = waterQualityBoundaryCondition->getWaterQualityConfiguration();
        waterQualityConfiguration->removeBoundaryCondition(waterQualityBoundaryCondition);
    }
    
    waterQualityBoundaryConditions.clear();
    
    this->boundaryConditions.removeOne(boundaryCondition);
    delete boundaryCondition;
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

QSet<WaterQualityConfiguration*> HydrodynamicConfiguration::getWaterQualityConfigurations() const {
    return waterQualityConfigurations;
}

void HydrodynamicConfiguration::addWaterQualityConfiguration(WaterQualityConfiguration *configuration) {
    waterQualityConfigurations.insert(configuration);
}

void HydrodynamicConfiguration::removeWaterQualityConfiguration(WaterQualityConfiguration *configuration) {
    waterQualityConfigurations.remove(configuration);
}

SimulationDataType::HydrodynamicConfiguration* HydrodynamicConfiguration::toSimulationDataType() const {
    SimulationDataType::HydrodynamicConfiguration *configuration = new SimulationDataType::HydrodynamicConfiguration();
    int i = 0;
    
    configuration->parameters = new SimulationDataType::HydrodynamicParameter[this->parameters.size()];
    configuration->numberOfParameters = this->parameters.size();
    
    for (HydrodynamicParameter *parameter : this->parameters) {
        QByteArray name = parameter->getName().toLocal8Bit();
        
        strncpy(configuration->parameters[i].name, name.constData(), name.size());
        configuration->parameters[i].value = parameter->getValue();
        i++;
    }
    
    configuration->boundaryConditions = new SimulationDataType::BoundaryCondition[this->boundaryConditions.size()];
    configuration->numberOfBoundaryConditions = this->boundaryConditions.size();
    i = 0;
    
    for (HydrodynamicBoundaryCondition *boundaryCondition : this->boundaryConditions) {
        configuration->boundaryConditions[i++] = boundaryCondition->toSimulationDataType(this->gridDataConfiguration->getMesh());
    }
    
    configuration->gridDataConfiguration = this->gridDataConfiguration->toSimulationDataType(this);
    
    return configuration;
}
