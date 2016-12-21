#ifndef WATER_QUALITY_CONFIGURATION_H
#define WATER_QUALITY_CONFIGURATION_H

#include "hydrodynamic_configuration.h"
#include "water_quality_parameter.h"
#include "water_quality_boundary_condition.h"
#include "food_matrix_value.h"

class WaterQualityConfiguration : QObject {
    Q_OBJECT
private:
    uint id;
    QString name;
    QList<WaterQualityParameter*> parameters;
    QList<WaterQualityBoundaryCondition*> boundaryConditions;
    QSet<FoodMatrixValue*> foodMatrix;
    HydrodynamicConfiguration *hydrodynamicConfiguration;
    
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
    HydrodynamicConfiguration* getHydrodynamicConfiguration() const;
    void setHydrodynamicConfiguration(HydrodynamicConfiguration *hydrodynamicConfiguration);
    bool addWaterQualityParameter(WaterQualityParameter *waterQualityParameter);
    QList<WaterQualityParameter*> getParameters(const bool &persistable = false) const;
    WaterQualityParameter* getParameter(const QString &name, const WaterQualityParameterSection &section) const;
    WaterQualityParameter* getParameterByDiagramItem(const QString &itemName) const;
    QList<WaterQualityParameter*> getRootParameters(const WaterQualityParameterSection &section) const;
    QList<WaterQualityBoundaryCondition*> getBoundaryConditions() const;
    void setBoundaryConditions(const QList<WaterQualityBoundaryCondition*> &boundaryConditions);
    void addBoundaryCondition(WaterQualityBoundaryCondition *boundaryCondition);
    void removeBoundaryCondition(WaterQualityBoundaryCondition *boundaryCondition);
    void clearBoundaryConditions();
    FoodMatrixValue* getFoodMatrixValue(const QString &predator, const int &predatorGroup, const QString &prey, const int &preyGroup) const;
    void addFoodMatrixValue(FoodMatrixValue *foodMatrixValue);
    void clearFoodMatrix();
    QSet<FoodMatrixValue*> getFoodMatrix() const;
    void setLoaded(const bool &loaded);
    bool isLoaded() const;
    SimulationDataType::WaterQualityConfiguration* toSimulationDataType() const;
};

#endif // WATER_QUALITY_CONFIGURATION_H
