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

QList<double> WaterQualityConfiguration::getFoodMatrixValues(const QString &predator, const QString &prey) const {
    QList<double> values = foodMatrix.values(QPair<QString, QString>(predator, prey));
    
    std::reverse(values.begin(), values.end());
    
    return values;
}

void WaterQualityConfiguration::setFoodMatrixItem(const QString &predator, const QString &prey, const double &value) {
    foodMatrix.insertMulti(QPair<QString, QString>(predator, prey), value);
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
    
    QList<QPair<QString, QString> > uniqueKeys = this->foodMatrix.uniqueKeys();
    int i = 0;
    
    configuration->numberOfFoodMatrixItems = uniqueKeys.size();
    configuration->foodMatrix = new SimulationDataType::FoodMatrixItem[uniqueKeys.size()];
    
    for (QPair<QString, QString> key : uniqueKeys) {
        QByteArray predator = key.first.toLocal8Bit();
        QByteArray prey = key.second.toLocal8Bit();
        QList<double> values = this->foodMatrix.values(key);
        
        configuration->foodMatrix[i] = SimulationDataType::FoodMatrixItem();
        
        strncpy(configuration->foodMatrix[i].predator, predator.constData(), predator.length());
        strncpy(configuration->foodMatrix[i].prey, prey.constData(), prey.length());
        
        configuration->foodMatrix[i].numberOfValues = values.size();
        configuration->foodMatrix[i].values = new double[values.size()];
        
        for (int j = 0; j < values.size(); j++) {
            configuration->foodMatrix[i].values[j] = values.at(j);
        }
        
        i++;
    }
    
    return configuration;
}