#include <domain/vertically_integrated_range.h>

VerticallyIntegratedRange::VerticallyIntegratedRange() :
    id(0),
    timeSeriesChanged(false)
{}

uint VerticallyIntegratedRange::getId() const {
    return id;
}

void VerticallyIntegratedRange::setId(const uint &id) {
    this->id = id;
}

bool VerticallyIntegratedRange::isPersisted() const {
    return id != 0;
}

double VerticallyIntegratedRange::getMinimumElevation() const {
    return minimumElevation;
}

void VerticallyIntegratedRange::setMinimumElevation(const double &minimumElevation) {
    this->minimumElevation = minimumElevation;
}

double VerticallyIntegratedRange::getMaximumElevation() const {
    return maximumElevation;
}

void VerticallyIntegratedRange::setMaximumElevation(const double &maximumElevation) {
    this->maximumElevation = maximumElevation;
}

BoundaryConditionFunction VerticallyIntegratedRange::getFunction() const {
    return function;
}

void VerticallyIntegratedRange::setFunction(const BoundaryConditionFunction &function) {
    this->function = function;
}

double VerticallyIntegratedRange::getValue() const {
    return value;
}

void VerticallyIntegratedRange::setValue(const double &value) {
    this->value = value;
}

QList<TimeSeries*> VerticallyIntegratedRange::getTimeSeriesList() const {
    return timeSeriesList;
}

QList<TimeSeries*>* VerticallyIntegratedRange::getTimeSeriesListPointer() {
    return &timeSeriesList;
}

void VerticallyIntegratedRange::setTimeSeriesList(const QList<TimeSeries*> &timeSeriesList) {
    this->timeSeriesList = timeSeriesList;
}

bool VerticallyIntegratedRange::addTimeSeries(TimeSeries *timeSeries) {
    if (timeSeriesList.contains(timeSeries)) {
        return false;
    }
    
    timeSeriesList.append(timeSeries);
    
    return true;
}

bool VerticallyIntegratedRange::isTimeSeriesChanged() const {
    return timeSeriesChanged;
}

void VerticallyIntegratedRange::setTimeSeriesChanged(bool timeSeriesChanged) {
    this->timeSeriesChanged = timeSeriesChanged;
}

SimulationDataType::VerticallyIntegratedRange VerticallyIntegratedRange::toSimulationDataType() const {
    SimulationDataType::VerticallyIntegratedRange range;
    
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
