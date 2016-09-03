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
    QList<FoodMatrix*> foodMatrixItems;
    GridDataConfiguration *gridDataConfiguration;
    
    // Transient
    bool loaded;
    
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
    QList<WaterQualityParameter*> getParameters(const bool &persistable = false) const;
    WaterQualityParameter* getParameter(const QString &name, const WaterQualityParameterSection &section) const;
    WaterQualityParameter* getParameterByDiagramItem(const QString &itemName) const;
    QList<WaterQualityParameter*> getRootParameters(const WaterQualityParameterSection &section) const;
    QList<FoodMatrix*> getFoodMatrixItems() const;
    void setFoodMatrixItems(const QList<FoodMatrix*> &foodMatrix);
    bool addFoodMatrixItem(FoodMatrix *foodMatrixItem);
    void setLoaded(const bool &loaded);
    bool isLoaded() const;
};

#endif // WATER_QUALITY_CONFIGURATION_H