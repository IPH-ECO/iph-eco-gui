#include <domain/water_quality_configuration.h>

WaterQualityConfiguration::WaterQualityConfiguration() :
    id(0),
    gridDataConfiguration(nullptr)
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

    for (WaterQualityParameter *parameter : parameters) {
        if (waterQualityParameter->isPersisted() && parameter->getId() == waterQualityParameter->getId()) {
            return false;
        }
    }
    
	parameters.append(waterQualityParameter);

	return true;
}

QList<WaterQualityParameter*> WaterQualityConfiguration::getParameters() const {
	return parameters;
}

WaterQualityParameter* WaterQualityConfiguration::getParameter(const QString &name) const {
	for (WaterQualityParameter *parameter : parameters) {
		if (parameter->getName() == name) {
			return parameter;
		}
	}

	return nullptr;
}

QList<FoodMatrix*> WaterQualityConfiguration::getFoodMatrix() const {
    return foodMatrix;
}

void WaterQualityConfiguration::setFoodMatrix(const QList<FoodMatrix*> &foodMatrix) {
    this->foodMatrix = foodMatrix;
}