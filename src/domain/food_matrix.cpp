#include <domain/food_matrix.h>

FoodMatrix::FoodMatrix(FoodMatrixElement *predator, FoodMatrixElement *prey, const double &value) :
    id(0),
    predator(predator),
    prey(prey),
    value(value)
{}

FoodMatrix::~FoodMatrix() {
    delete predator;
    delete prey;
}

uint FoodMatrix::getId() const {
    return id;
}

void FoodMatrix::setId(const uint &id) {
    this->id = id;
}

FoodMatrixElement* FoodMatrix::getPredator() const {
    return predator;
}

void FoodMatrix::setPredator(FoodMatrixElement *predator) {
    this->predator = predator;
}

FoodMatrixElement* FoodMatrix::getPrey() const {
    return prey;
}

void FoodMatrix::setPrey(FoodMatrixElement *prey) {
    this->prey = prey;
}

double FoodMatrix::getValue() const {
    return value;
}

void FoodMatrix::setValue(const double &value) {
    this->value = value;
}