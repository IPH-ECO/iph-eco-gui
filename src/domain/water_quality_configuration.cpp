#include <domain/water_quality_configuration.h>

WaterQualityConfiguration::WaterQualityConfiguration() :
    id(0),
    hydrodynamicConfiguration(nullptr),
    loaded(false)
{}

WaterQualityConfiguration::~WaterQualityConfiguration() {
    for (WaterQualityParameter *parameter : parameters) {
        delete parameter;
    }
    
    for (FoodMatrixValue *foodMatrixValue : foodMatrix) {
        delete foodMatrixValue;
    }
}

uint WaterQualityConfiguration::getId() const {
	return id;
}

void WaterQualityConfiguration::setId(const uint &id) {
	this->id = id;
}

bool WaterQualityConfiguration::isPersisted() const {
	return id != 0;
}

QString WaterQualityConfiguration::getName() const {
	return name;
}

void WaterQualityConfiguration::setName(const QString &name) {
	this->name = name;
}

HydrodynamicConfiguration* WaterQualityConfiguration::getHydrodynamicConfiguration() const {
	return hydrodynamicConfiguration;
}

void WaterQualityConfiguration::setHydrodynamicConfiguration(HydrodynamicConfiguration *hydrodynamicConfiguration) {
    if (this->hydrodynamicConfiguration && this->hydrodynamicConfiguration != hydrodynamicConfiguration) {
        this->hydrodynamicConfiguration->removeWaterQualityConfiguration(this);
    }
    
	this->hydrodynamicConfiguration = hydrodynamicConfiguration;
    this->hydrodynamicConfiguration->addWaterQualityConfiguration(this);
}

bool WaterQualityConfiguration::addWaterQualityParameter(WaterQualityParameter *waterQualityParameter) {
	if (parameters.contains(waterQualityParameter)) {
		return false;
	}

	parameters.append(waterQualityParameter);

	return true;
}

QList<WaterQualityParameter*> WaterQualityConfiguration::getParameters(const bool &persistable) const {
    if (persistable) {
        QList<WaterQualityParameter*> persistableParameters;
        
        for (WaterQualityParameter *parameter : parameters) {
            if (parameter->isPersistable()) {
                persistableParameters.append(parameter);
            }
        }
        
        return persistableParameters;
    }
    
	return parameters;
}

WaterQualityParameter* WaterQualityConfiguration::getParameter(const QString &name, const WaterQualityParameterSection &section) const {
	for (WaterQualityParameter *parameter : parameters) {
		if (parameter->getName() == name && parameter->getSection() == section) {
			return parameter;
		}
	}

	return nullptr;
}

WaterQualityParameter* WaterQualityConfiguration::getParameterByDiagramItem(const QString &itemName) const {
    for (WaterQualityParameter *parameter : parameters) {
        if (parameter->getSection() == WaterQualityParameterSection::STRUCTURE && parameter->getDiagramItem() == itemName) {
            return parameter;
        }
    }
    
    return nullptr;
}

QList<WaterQualityParameter*> WaterQualityConfiguration::getRootParameters(const WaterQualityParameterSection &section) const {
    QList<WaterQualityParameter*> rootParameters;
    
    for (WaterQualityParameter *parameter : parameters) {
        if (parameter->getSection() == section && !parameter->getParent()) {
            rootParameters.append(parameter);
        }
    }
    
    return rootParameters;
}

QList<WaterQualityBoundaryCondition*> WaterQualityConfiguration::getBoundaryConditions() const {
    return boundaryConditions;
}

void WaterQualityConfiguration::setBoundaryConditions(const QList<WaterQualityBoundaryCondition*> &boundaryConditions) {
    this->boundaryConditions = boundaryConditions;
}

void WaterQualityConfiguration::addBoundaryCondition(WaterQualityBoundaryCondition *boundaryCondition) {
    for (WaterQualityBoundaryCondition *existentBoundaryCondition : this->boundaryConditions) {
        if (existentBoundaryCondition->getName() == boundaryCondition->getName()) {
            return;
        }
    }
    
    boundaryConditions.append(boundaryCondition);
}

void WaterQualityConfiguration::removeBoundaryCondition(WaterQualityBoundaryCondition *boundaryCondition) {
    boundaryConditions.removeOne(boundaryCondition);
    delete boundaryCondition;
}

void WaterQualityConfiguration::clearBoundaryConditions() {
    for (BoundaryCondition *boundaryCondition : boundaryConditions) {
        delete boundaryCondition;
    }
    
    boundaryConditions.clear();
}

FoodMatrixValue* WaterQualityConfiguration::getFoodMatrixValue(const QString &predator, const int &predatorGroup, const QString &prey, const int &preyGroup) const {
    for (FoodMatrixValue *foodMatrixValue : foodMatrix) {
        if (predator == foodMatrixValue->predator && predatorGroup == foodMatrixValue->predatorGroup && prey == foodMatrixValue->prey && preyGroup == foodMatrixValue->preyGroup) {
            return foodMatrixValue;
        }
    }
    
    return nullptr;
}

void WaterQualityConfiguration::addFoodMatrixValue(FoodMatrixValue *foodMatrixValue) {
    foodMatrix.insert(foodMatrixValue);
}

void WaterQualityConfiguration::clearFoodMatrix() {
    foodMatrix.clear();
}

QSet<FoodMatrixValue*> WaterQualityConfiguration::getFoodMatrix() const {
    return foodMatrix;
}

void WaterQualityConfiguration::setLoaded(const bool &loaded) {
    this->loaded = loaded;
}

bool WaterQualityConfiguration::isLoaded() const {
    return loaded;
}

SimulationDataType::WaterQualityConfiguration* WaterQualityConfiguration::toSimulationDataType() const {
    SimulationDataType::WaterQualityConfiguration *configuration = new SimulationDataType::WaterQualityConfiguration();
    QList<WaterQualityParameter*> persistableParameters = this->getParameters(true);
    int i = 0;
    
    configuration->numberOfParameters = persistableParameters.size();
    configuration->parameters = new SimulationDataType::WaterQualityParameter[configuration->numberOfParameters];
    
    while (i < configuration->numberOfParameters) {
        configuration->parameters[i] = persistableParameters[i]->toSimulationDataType();
        i++;
    }
    
    configuration->foodMatrixSize = this->foodMatrix.size();
    configuration->foodMatrix = new SimulationDataType::FoodMatrixValue[configuration->foodMatrixSize];
    i = 0;
    
    for (FoodMatrixValue *foodMatrixValue : foodMatrix)  {
        QByteArray predator = foodMatrixValue->predator.toLocal8Bit();
        QByteArray prey = foodMatrixValue->prey.toLocal8Bit();
        
        strncpy(configuration->foodMatrix[i].predator, predator.constData(), predator.length());
        configuration->foodMatrix[i].predatorGroup = foodMatrixValue->predatorGroup;
        strncpy(configuration->foodMatrix[i].prey, prey.constData(), prey.length());
        configuration->foodMatrix[i].preyGroup = foodMatrixValue->preyGroup;
        configuration->foodMatrix[i].value = foodMatrixValue->value;
        
        i++;
    }
    
    return configuration;
}
