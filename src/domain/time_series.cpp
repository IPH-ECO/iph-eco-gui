#include "include/domain/time_series.h"

TimeSeries::TimeSeries() : id(0) {}

uint TimeSeries::getId() const {
	return id;
}

void TimeSeries::setId(uint id) {
	if (!isPersisted()) {
		this->id = id;
	}
}

bool TimeSeries::isPersisted() const {
	return id != 0;
}

int TimeSeries::getTimeStamp() const {
	return timeStamp;
}

void TimeSeries::setTimeStamp(int timeStamp) {
	this->timeStamp = timeStamp;
}

QDateTime TimeSeries::toDateTime() const {
	return QDateTime::fromTime_t(timeStamp);
}

double TimeSeries::getValue() const {
	return value;
}

void TimeSeries::setValue(double value) {
	this->value = value;
}

SimulationDataType::TimeSeries TimeSeries::toSimulationDataType() const {
    SimulationDataType::TimeSeries timeSeries;
    
    timeSeries.timeStamp = this->timeStamp;
    timeSeries.value = this->value;
    
    return timeSeries;
}