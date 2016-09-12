#ifndef FOOD_MATRIX_ELEMENT_H
#define FOOD_MATRIX_ELEMENT_H

#include <domain/water_quality_parameter.h>
#include <QList>

class FoodMatrixElement {
private:
    QString name;
    QString label;
    WaterQualityParameter *group;
    WaterQualityParameter *parameter;
    QList<FoodMatrixElement*> preys;
    QList<FoodMatrixElement*> predators;

public:
    FoodMatrixElement();
    QString getName() const;
    void setName(const QString &name);
    QString getLabel() const;
    void setLabel(const QString &label);
    WaterQualityParameter* getGroup() const;
    void setGroup(WaterQualityParameter *group);
    WaterQualityParameter* getParameter() const;
    void setParameter(WaterQualityParameter *parameter);
    QList<FoodMatrixElement*> getPreys() const;
    void setPreys(const QList<FoodMatrixElement*> &preys);
    void addPrey(FoodMatrixElement *prey);
    QList<FoodMatrixElement*> getPredators() const;
    void setPredators(const QList<FoodMatrixElement*> &preys);
    void addPredator(FoodMatrixElement *prey);
};

#endif // FOOD_MATRIX_ELEMENT_H