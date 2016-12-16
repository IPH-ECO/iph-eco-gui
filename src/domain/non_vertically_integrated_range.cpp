#include <domain/non_vertically_integrated_range.h>

NonVerticallyIntegratedRange::NonVerticallyIntegratedRange() :
    id(0),
    timeSeriesChanged(false)
{}

uint NonVerticallyIntegratedRange::getId() const {
    return id;
}

void NonVerticallyIntegratedRange::setId(const uint &id) {
    this->id = id;
}

bool NonVerticallyIntegratedRange::isPersisted() const {
    return id != 0;
}

double NonVerticallyIntegratedRange::getMinimumElevation() const {
    return minimumElevation;
}

void NonVerticallyIntegratedRange::setMinimumElevation(const double &minimumElevation) {
    this->minimumElevation = minimumElevation;
}

double NonVerticallyIntegratedRange::getMaximumElevation() const {
    return maximumElevation;
}

void NonVerticallyIntegratedRange::setMaximumElevation(const double &maximumElevation) {
    this->maximumElevation = maximumElevation;
}

BoundaryConditionFunction NonVerticallyIntegratedRange::getFunction() const {
    return function;
}

void NonVerticallyIntegratedRange::setFunction(const BoundaryConditionFunction &function) {
    this->function = function;
}

double NonVerticallyIntegratedRange::getValue() const {
    return value;
}

void NonVerticallyIntegratedRange::setValue(const double &value) {
    this->value = value;
}

QList<TimeSeries*> NonVerticallyIntegratedRange::getTimeSeriesList() const {
    return timeSeriesList;
}

QList<TimeSeries*>* NonVerticallyIntegratedRange::getTimeSeriesListPointer() {
    return &timeSeriesList;
}

void NonVerticallyIntegratedRange::setTimeSeriesList(const QList<TimeSeries*> &timeSeriesList) {
    this->timeSeriesList = timeSeriesList;
}

bool NonVerticallyIntegratedRange::addTimeSeries(TimeSeries *timeSeries) {
    if (timeSeriesList.contains(timeSeries)) {
        return false;
    }
    
    timeSeriesList.append(timeSeries);
    
    return true;
}

bool NonVerticallyIntegratedRange::isTimeSeriesChanged() const {
    return timeSeriesChanged;
}

void NonVerticallyIntegratedRange::setTimeSeriesChanged(bool timeSeriesChanged) {
    this->timeSeriesChanged = timeSeriesChanged;
}

SimulationDataType::NonVerticallyIntegratedRange NonVerticallyIntegratedRange::toSimulationDataType() const {
    SimulationDataType::NonVerticallyIntegratedRange range;
    
    range.minimumElevation = this->minimumElevation;
    range.maximumElevation = this->maximumElevation;
    range.function = (int) this->function;
    
    if (this->function == BoundaryConditionFunction::CONSTANT) {
        range.value = this->value;
    } else {
        range.timeSeriesListSize = this->timeSeriesList.size();
        range.timeSeriesList = new SimulationDataType::TimeSeries[range.timeSeriesListSize];
        
        for (int i = 0; i < range.timeSeriesListSize; i++) {
            range.timeSeriesList[i] = this->timeSeriesList[i]->toSimulationDataType();
        }
    }
    
    return range;
}
