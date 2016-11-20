#ifndef WATER_QUALITY_REPOSITORY_H
#define WATER_QUALITY_REPOSITORY_H

#include <utility/database_utility.h>
#include <domain/water_quality_parameter.h>
#include <domain/water_quality_configuration.h>
#include <domain/food_matrix_item.h>

#include <QJsonArray>

class WaterQualityRepository {
private:
    static WaterQualityRepository *instance;
    
    QJsonArray jsonStructure;
    QJsonArray jsonParameters;
    QJsonArray jsonFoodMatrix;
    QJsonArray jsonInitialConditions;
    QJsonArray jsonVariables;
    QList<FoodMatrixElement*> predators;
    QList<FoodMatrixElement*> preys;
    QList<FoodMatrixItem> defaultFoodMatrix;
    QList<WaterQualityParameter*> boundaryConditionVariables;
    
    WaterQualityRepository();

    FoodMatrixElement* findPreyByName(const QString &name);
    QStringList findGroups(WaterQualityConfiguration *configuration, const QString &parentGroupName) const;
public:
    static WaterQualityRepository* getInstance();
    
    ~WaterQualityRepository();
    
    void loadParameters(WaterQualityConfiguration *configuration);
    QList<FoodMatrixElement*> getPredators() const;
    QList<FoodMatrixElement*> getPreys() const;
    double getDefaultFoodMatrixValue(FoodMatrixElement *predator, FoodMatrixElement *prey) const;
    QList<WaterQualityParameter*> getBoundaryConditionVariables() const;
};

#endif // WATER_QUALITY_REPOSITORY_H