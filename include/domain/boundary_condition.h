#ifndef BOUNDARY_CONDITION_H
#define BOUNDARY_CONDITION_H

#include "simulation_data_type.h"
#include "time_series.h"
#include "mesh.h"
#include "non_vertically_integrated_range.h"

enum class BoundaryConditionType { WATER_LEVEL = 1, WATER_FLOW, NORMAL_DEPTH };
enum class InputModule { HYDRODYNAMIC = 1, WATER_QUALITY };

class BoundaryCondition : public QObject {
    Q_OBJECT
protected:
    uint id;
    QString name;
    BoundaryConditionType type;
    BoundaryConditionFunction function;
    bool verticallyIntegrated;
    double constantValue;
    InputModule inputModule;
    QList<TimeSeries*> timeSeriesList;
    QSet<NonVerticallyIntegratedRange*> nonVerticallyIntegratedRanges;
    
    // Transient attributes
    bool timeSeriesChanged;

public:
    BoundaryCondition();
    virtual ~BoundaryCondition();

    uint getId() const;
    void setId(const uint &id);
    QString getName() const;
    void setName(const QString &name);
    bool isPersisted() const;
    BoundaryConditionType getType() const;
    void setType(const BoundaryConditionType &type);
    BoundaryConditionFunction getFunction() const;
    void setFunction(const BoundaryConditionFunction &function);
    bool isVerticallyIntegrated() const;
    void setVerticallyIntegrated(const bool &verticallyIntegrated);
    double getConstantValue() const;
    void setConstantValue(double constantValue);
    InputModule getInputModule() const;
    void setInputModule(const InputModule &inputModule);
    QList<TimeSeries*> getTimeSeriesList() const;
    QList<TimeSeries*>* getTimeSeriesListPointer();
    void setTimeSeriesList(const QList<TimeSeries*> &timeSeriesList);
    bool addTimeSeries(TimeSeries *timeSeries);
    QSet<NonVerticallyIntegratedRange*> getNonVerticallyIntegratedRanges() const;
    void setNonVerticallyIntegratedRanges(const QSet<NonVerticallyIntegratedRange*> &nonVerticallyIntegratedRanges);
    void addNonVerticallyIntegratedRange(NonVerticallyIntegratedRange *nonVerticallyIntegratedRanges);
    void removeNonVerticallyIntegratedRange(NonVerticallyIntegratedRange *nonVerticallyIntegratedRanges);
    QString getObjectTypeLabel() const;
    QString getTypeLabel() const;
    QString getFunctionLabel() const;
    bool isTimeSeriesChanged() const;
    void setTimeSeriesChanged(const bool &timeSeriesChanged);
};

#endif // BOUNDARY_CONDITION_H
