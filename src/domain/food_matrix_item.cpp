#include <domain/food_matrix_item.h>

FoodMatrixItem::FoodMatrixItem(FoodMatrixElement *predator, FoodMatrixElement *prey, const double &value) :
    predator(predator),
    prey(prey),
    value(value)
{}

FoodMatrixItem::FoodMatrixItem() {
    delete predator;
    delete prey;
}

FoodMatrixElement* FoodMatrixItem::getPredator() const {
    return predator;
}

void FoodMatrixItem::setPredator(FoodMatrixElement *predator) {
    this->predator = predator;
}

FoodMatrixElement* FoodMatrixItem::getPrey() const {
    return prey;
}

void FoodMatrixItem::setPrey(FoodMatrixElement *prey) {
    this->prey = prey;
}

double FoodMatrixItem::getValue() const {
    return value;
}

void FoodMatrixItem::setValue(const double &value) {
    this->value = value;
}