#ifndef WATER_QUALITY_CONFIGURATION_H
#define WATER_QUALITY_CONFIGURATION_H

#include "grid_data_configuration.h"
#include "water_quality_parameter.h"
#include "food_matrix.h"

class WaterQualityConfiguration {
private:
    uint id;
    QString name;
    QList<WaterQualityParameter*> parameters;
    QList<FoodMatrix*> foodMatrix;
    GridDataConfiguration *gridDataConfiguration;
public:
    WaterQualityConfiguration();
    ~WaterQualityConfiguration();

    uint getId() const;
    void setId(const uint &id);
    bool isPersisted() const;
    QString getName() const;
    void setName(const QString &name);
    GridDataConfiguration* getGridDataConfiguration() const;
    void setGridDataConfiguration(GridDataConfiguration *gridDataConfiguration);
    bool addWaterQualityParameter(WaterQualityParameter *waterQualityParameter);
    QList<WaterQualityParameter*> getParameters() const;
    WaterQualityParameter* getParameter(const QString &name) const;
    QList<FoodMatrix*> getFoodMatrix() const;
    void setFoodMatrix(const QList<FoodMatrix*> &foodMatrix);
};

#endif // WATER_QUALITY_CONFIGURATION_H