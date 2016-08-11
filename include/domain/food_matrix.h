#ifndef FOOD_MATRIX_H
#define FOOD_MATRIX_H

#include <domain/food_matrix_element.h>

class FoodMatrix {
private:
    uint id;
    FoodMatrixElement *predator;
    FoodMatrixElement *prey;
    double value;
public:
    FoodMatrix(FoodMatrixElement *predator, FoodMatrixElement *prey, const double &value);
    uint getId() const;
    void setId(const uint &id);
    FoodMatrixElement* getPredator() const;
    void setPredator(FoodMatrixElement *predator);
    FoodMatrixElement* getPrey() const;
    void setPrey(FoodMatrixElement *prey);
    double getValue() const;
    void setValue(const double &value);
};

#endif // FOOD_MATRIX_H