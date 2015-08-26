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

QString TimeSeries::getTimeStamp() const {
	return timeStamp;
}

void TimeSeries::setTimeStamp(const QString &timeStamp) {
	this->timeStamp = timeStamp;
}

QDateTime TimeSeries::toDateTime() const {
	return QDateTime::fromString(timeStamp, Qt::ISODate);
}

double TimeSeries::getValue() const {
	return value;
}

void TimeSeries::setValue(double value) {
	this->value = value;
}

SimulationDataType::TimeSeries TimeSeries::toSimulationDataType() const {
    SimulationDataType::TimeSeries timeSeries;
    std::string timeStampStr = timeStamp.toStdString();
    
    timeSeries.timeStampSize = timeStamp.size();
    strncpy(timeSeries.timeStamp, timeStampStr.c_str(), timeSeries.timeStampSize);
    
    return timeSeries;
}