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
	this->hydrodynamicConfiguration = hydrodynamicConfiguration;
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

double WaterQualityConfiguration::getFoodMatrixValue(const QString &predator, const QString &prey) const {
    return foodMatrix.value(QPair<QString, QString>(predator, prey), std::numeric_limits<double>::max());
}

void WaterQualityConfiguration::setFoodMatrixItem(const QString &predator, const QString &prey, const double &value) {
    foodMatrix.insert(QPair<QString, QString>(predator, prey), value);
}

void WaterQualityConfiguration::clearFoodMatrix() {
    foodMatrix.clear();
}

QHash<QPair<QString, QString>, double> WaterQualityConfiguration::getFoodMatrix() const {
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
    
    configuration->numberOfParameters = persistableParameters.size();
    configuration->parameters = new SimulationDataType::WaterQualityParameter[configuration->numberOfParameters];
    
    for (int i = 0; i < configuration->numberOfParameters; i++) {
        configuration->parameters[i] = persistableParameters[i]->toSimulationDataType();
    }
    
    int i = 0;
    
    configuration->foodMatrix = new SimulationDataType::FoodMatrixItem[this->foodMatrix.size()];
                                                                       
    for (QHash<QPair<QString, QString>, double>::const_iterator it = foodMatrix.constBegin(); it != foodMatrix.constEnd(); it++) {
        QByteArray predator = it.key().first.toLocal8Bit();
        QByteArray prey = it.key().second.toLocal8Bit();
        
        configuration->foodMatrix[i] = SimulationDataType::FoodMatrixItem();
        
        strncpy(configuration->foodMatrix[i].predator, predator.constData(), predator.size());
        strncpy(configuration->foodMatrix[i].prey, prey.constData(), prey.size());
        
        configuration->foodMatrix[i].value = it.value();
    }
    
    return configuration;
}