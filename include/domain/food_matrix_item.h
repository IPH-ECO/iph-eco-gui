#ifndef FOOD_MATRIX_ITEM_H
#define FOOD_MATRIX_ITEM_H

#include <domain/food_matrix_element.h>

class FoodMatrixItem {
private:
    FoodMatrixElement *predator;
    FoodMatrixElement *prey;
    double value;
public:
    FoodMatrixItem(FoodMatrixElement *predator, FoodMatrixElement *prey, const double &value);
    FoodMatrixItem();
    FoodMatrixElement* getPredator() const;
    void setPredator(FoodMatrixElement *predator);
    FoodMatrixElement* getPrey() const;
    void setPrey(FoodMatrixElement *prey);
    double getValue() const;
    void setValue(const double &value);
};

#endif // FOOD_MATRIX_ITEM_H