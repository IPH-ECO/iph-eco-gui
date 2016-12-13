#include <domain/vertical_integrated_range.h>

VerticalIntegratedRange::VerticalIntegratedRange() :
    id(0),
    timeSeriesChanged(false)
{}

uint VerticalIntegratedRange::getId() const {
    return id;
}

void VerticalIntegratedRange::setId(const uint &id) {
    this->id = id;
}

bool VerticalIntegratedRange::isPersisted() const {
    return id != 0;
}

double VerticalIntegratedRange::getMinimumElevation() const {
    return minimumElevation;
}

void VerticalIntegratedRange::setMinimumElevation(const double &minimumElevation) {
    this->minimumElevation = minimumElevation;
}

double VerticalIntegratedRange::getMaximumElevation() const {
    return maximumElevation;
}

void VerticalIntegratedRange::setMaximumElevation(const double &maximumElevation) {
    this->maximumElevation = maximumElevation;
}

BoundaryConditionFunction VerticalIntegratedRange::getFunction() const {
    return function;
}

void VerticalIntegratedRange::setFunction(const BoundaryConditionFunction &function) {
    this->function = function;
}

double VerticalIntegratedRange::getValue() const {
    return value;
}

void VerticalIntegratedRange::setValue(const double &value) {
    this->value = value;
}

QList<TimeSeries*> VerticalIntegratedRange::getTimeSeriesList() const {
    return timeSeriesList;
}

QList<TimeSeries*>* VerticalIntegratedRange::getTimeSeriesListPointer() {
    return &timeSeriesList;
}

void VerticalIntegratedRange::setTimeSeriesList(const QList<TimeSeries*> &timeSeriesList) {
    this->timeSeriesList = timeSeriesList;
}

bool VerticalIntegratedRange::addTimeSeries(TimeSeries *timeSeries) {
    if (timeSeriesList.contains(timeSeries)) {
        return false;
    }
    
    timeSeriesList.append(timeSeries);
    
    return true;
}

bool VerticalIntegratedRange::isTimeSeriesChanged() const {
    return timeSeriesChanged;
}

void VerticalIntegratedRange::setTimeSeriesChanged(bool timeSeriesChanged) {
    this->timeSeriesChanged = timeSeriesChanged;
}
