#ifndef VERTICALLY_INTEGRATED_RANGE_H
#define VERTICALLY_INTEGRATED_RANGE_H

#include "boundary_condition.h"

class VerticallyIntegratedRange {
private:
    uint id;
    double minimumElevation;
    double maximumElevation;
    BoundaryConditionFunction function;
    double value;
    QList<TimeSeries*> timeSeriesList;

    // Transient attributes
    bool timeSeriesChanged;
    
public:
    VerticallyIntegratedRange();

    uint getId() const;
    void setId(const uint &id);
    bool isPersisted() const;
    double getMinimumElevation() const;
    void setMinimumElevation(const double &minimumElevation);
    double getMaximumElevation() const;
    void setMaximumElevation(const double &maximumElevation);
    BoundaryConditionFunction getFunction() const;
    void setFunction(const BoundaryConditionFunction &function);
    double getValue() const;
    void setValue(const double &value);
    QList<TimeSeries*> getTimeSeriesList() const;
    QList<TimeSeries*>* getTimeSeriesListPointer();
    void setTimeSeriesList(const QList<TimeSeries*> &timeSeriesList);
    bool addTimeSeries(TimeSeries *timeSeries);
    bool isTimeSeriesChanged() const;
    void setTimeSeriesChanged(bool timeSeriesChanged);
    
    SimulationDataType::VerticallyIntegratedRange toSimulationDataType() const;
};

#endif // VERTICALLY_INTEGRATED_RANGE_H
