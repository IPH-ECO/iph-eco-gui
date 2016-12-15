#ifndef VERTICAL_INTEGRATED_RANGE_H
#define VERTICAL_INTEGRATED_RANGE_H

#include "boundary_condition.h"

class VerticalIntegratedRange {
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
    VerticalIntegratedRange();

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
    
    SimulationDataType::VerticalIntegratedRange toSimulationDataType() const;
};

#endif // VERTICAL_INTEGRATED_RANGE_H
