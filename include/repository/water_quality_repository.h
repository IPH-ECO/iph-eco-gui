#ifndef WATER_QUALITY_REPOSITORY_H
#define WATER_QUALITY_REPOSITORY_H

#include <utility/database_utility.h>
#include <domain/water_quality_parameter.h>
#include <domain/water_quality_configuration.h>
#include <domain/food_matrix.h>

class WaterQualityRepository {
private:
    static WaterQualityRepository *instance;
    
    QList<WaterQualityParameter*> parameters;
    QList<WaterQualityParameter*> structure;
    QList<WaterQualityParameter*> rootParameters;
    QList<WaterQualityParameter*> rootStructure;
    QList<FoodMatrixElement*> predators;
    QList<FoodMatrixElement*> preys;
    QList<FoodMatrix> foodMatrix;
    
    WaterQualityRepository();

    FoodMatrixElement* findPreyByName(const QString &name);
public:
    static WaterQualityRepository* getInstance();
    ~WaterQualityRepository();

    WaterQualityParameter* findParameterByName(const QString &name, const WaterQualityParameterSection &section);
    WaterQualityParameter* findParameterByDiagramItem(const QString &diagramItem);
    QList<WaterQualityParameter*> getParameters(const WaterQualityParameterSection &section) const;
    QList<WaterQualityParameter*> getRootParameters(const WaterQualityParameterSection &section) const;
    QList<FoodMatrixElement*> getPredators() const;
    QList<FoodMatrixElement*> getPreys() const;
    double getFoodMatrixValue(FoodMatrixElement *predator, FoodMatrixElement *prey) const;
};

#endif // WATER_QUALITY_REPOSITORY_H