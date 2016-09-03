#ifndef WATER_QUALITY_REPOSITORY_H
#define WATER_QUALITY_REPOSITORY_H

#include <utility/database_utility.h>
#include <domain/water_quality_parameter.h>
#include <domain/water_quality_configuration.h>
#include <domain/food_matrix.h>

#include <QJsonArray>

class WaterQualityRepository {
private:
    static WaterQualityRepository *instance;
    
    QJsonArray jsonStructure;
    QJsonArray jsonParameters;
    QJsonArray jsonFoodMatrix;
    QList<FoodMatrixElement*> predators;
    QList<FoodMatrixElement*> preys;
    QList<FoodMatrix> foodMatrix;
    
    WaterQualityRepository();

    FoodMatrixElement* findPreyByName(const QString &name);
public:
    static WaterQualityRepository* getInstance();
    
    void loadParameters(WaterQualityConfiguration *configuration);
    QList<FoodMatrixElement*> getPredators() const;
    QList<FoodMatrixElement*> getPreys() const;
    double getFoodMatrixValue(FoodMatrixElement *predator, FoodMatrixElement *prey) const;
};

#endif // WATER_QUALITY_REPOSITORY_H