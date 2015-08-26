#ifndef TIME_SERIES_H
#define TIME_SERIES_H

#include "simulation_data_type.h"

#include <QDateTime>

class TimeSeries {
private:
	uint id;
	QString timeStamp;
	double value;
public:
	TimeSeries();
	uint getId() const;
	void setId(uint id);
	bool isPersisted() const;
	QString getTimeStamp() const;
	void setTimeStamp(const QString &timeStamp);
	QDateTime toDateTime() const;
	double getValue() const;
	void setValue(double value);
    SimulationDataType::TimeSeries toSimulationDataType() const;
};

#endif // TIME_SERIES_H