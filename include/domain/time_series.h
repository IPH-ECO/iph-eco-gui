#ifndef TIME_SERIES_H
#define TIME_SERIES_H

#include "simulation_data_type.h"

#include <QDateTime>

class TimeSeries {
private:
	uint id;
	int timeStamp;
	double value;
public:
	TimeSeries();
	uint getId() const;
	void setId(uint id);
	bool isPersisted() const;
	int getTimeStamp() const;
	void setTimeStamp(int timeStamp);
	QDateTime toDateTime() const;
	double getValue() const;
	void setValue(double value);
    SimulationDataType::TimeSeries toSimulationDataType() const;
};

#endif // TIME_SERIES_H