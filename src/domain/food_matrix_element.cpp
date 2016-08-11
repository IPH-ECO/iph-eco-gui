#include <domain/food_matrix_element.h>

FoodMatrixElement::FoodMatrixElement() :
	id(0),
	group(nullptr),
	parameter(nullptr)
{}

uint FoodMatrixElement::getId() const {
    return id;
}

void FoodMatrixElement::setId(const uint &id) {
    this->id = id;
}

QString FoodMatrixElement::getName() const {
    return name;
}

void FoodMatrixElement::setName(const QString &name) {
    this->name = name;
}

QString FoodMatrixElement::getLabel() const {
    return label;
}

void FoodMatrixElement::setLabel(const QString &label) {
    this->label = label;
}

WaterQualityParameter* FoodMatrixElement::getGroup() const {
    return group;
}

void FoodMatrixElement::setGroup(WaterQualityParameter *group) {
    this->group = group;
}

WaterQualityParameter* FoodMatrixElement::getParameter() const {
    return parameter;
}

void FoodMatrixElement::setParameter(WaterQualityParameter *parameter) {
    this->parameter = parameter;
}

QList<FoodMatrixElement*> FoodMatrixElement::getPreys() const {
    return preys;
}

void FoodMatrixElement::setPreys(const QList<FoodMatrixElement*> &preys) {
    this->preys = preys;
}

void FoodMatrixElement::addPrey(FoodMatrixElement *prey) {
    if (prey) {
        prey->addPredator(this);
        
        if (!preys.contains(prey)) {
            preys.append(prey);
        }
	}
}

QList<FoodMatrixElement*> FoodMatrixElement::getPredators() const {
    return predators;
}

void FoodMatrixElement::setPredators(const QList<FoodMatrixElement*> &predators) {
    this->predators = predators;
}

void FoodMatrixElement::addPredator(FoodMatrixElement *predator) {
    if (!predators.contains(predator)) {
        predators.append(predator);
    }
}
