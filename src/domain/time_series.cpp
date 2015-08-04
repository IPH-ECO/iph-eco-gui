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

QString TimeSeries::getTimestamp() const {
	return timestamp;
}

void TimeSeries::setTimestamp(const QString &timestamp) {
	this->timestamp = timestamp;
}

QDateTime TimeSeries::toDateTime() const {
	return QDateTime::fromString(timestamp, Qt::ISODate);
}

double TimeSeries::getValue() const {
	return value;
}

void TimeSeries::setValue(double value) {
	this->value = value;
}