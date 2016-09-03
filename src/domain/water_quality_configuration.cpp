#include <domain/water_quality_configuration.h>

WaterQualityConfiguration::WaterQualityConfiguration() :
    id(0),
    gridDataConfiguration(nullptr),
    loaded(false)
{}

WaterQualityConfiguration::~WaterQualityConfiguration() {
    for (WaterQualityParameter *parameter : parameters) {
        delete parameter;
    }
    
    for (FoodMatrix *foodMatrixItem : foodMatrixItems) {
        delete foodMatrixItem;
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

GridDataConfiguration* WaterQualityConfiguration::getGridDataConfiguration() const {
	return gridDataConfiguration;
}

void WaterQualityConfiguration::setGridDataConfiguration(GridDataConfiguration *gridDataConfiguration) {
	this->gridDataConfiguration = gridDataConfiguration;
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

QList<FoodMatrix*> WaterQualityConfiguration::getFoodMatrixItems() const {
    return foodMatrixItems;
}

void WaterQualityConfiguration::setFoodMatrixItems(const QList<FoodMatrix*> &foodMatrixItems) {
    this->foodMatrixItems = foodMatrixItems;
}

bool WaterQualityConfiguration::addFoodMatrixItem(FoodMatrix *foodMatrixItem) {
    if (foodMatrixItems.contains(foodMatrixItem)) {
        return false;
    }
    
    foodMatrixItems.append(foodMatrixItem);
    
    return true;
}

void WaterQualityConfiguration::setLoaded(const bool &loaded) {
    this->loaded = loaded;
}

bool WaterQualityConfiguration::isLoaded() const {
    return loaded;
}