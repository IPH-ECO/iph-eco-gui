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
    return QDateTime::fromTime_t(timeStamp, Qt::UTC);
}

double TimeSeries::getValue1() const {
	return value1;
}

void TimeSeries::setValue1(double value1) {
	this->value1 = value1;
}

double TimeSeries::getValue2() const {
    return value2;
}

void TimeSeries::setValue2(double value2) {
    this->value2 = value2;
}

uint TimeSeries::getObjectId() const {
    return objectId;
}

void TimeSeries::setObjectId(uint objectId) {
    this->objectId = objectId;
}

QString TimeSeries::getObjectType() const {
    return objectType;
}

void TimeSeries::setObjectType(const QString &objectType) {
    this->objectType = objectType;
}

SimulationDataType::TimeSeries TimeSeries::toSimulationDataType() const {
    SimulationDataType::TimeSeries timeSeries;
    
    timeSeries.timeStamp = this->timeStamp;
    timeSeries.value1 = this->value1;
    timeSeries.value2 = this->value2;
    
    return timeSeries;
}

TimeSeriesType TimeSeries::mapStringToEnumType(const QString &typeStr) {
    if (typeStr == "XY Components") {
        return TimeSeriesType::XY_COMPONENTS;
    }
    
    if (typeStr == "Intensity and Direction") {
        return TimeSeriesType::INTENSITY_DIRECTION;
    }
    
    return TimeSeriesType::DEFAULT;
}