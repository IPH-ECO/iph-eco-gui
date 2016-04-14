#ifndef TIME_SERIES_H
#define TIME_SERIES_H

#include "simulation_data_type.h"

#include <QDateTime>

enum class TimeSeriesType { DEFAULT = 1, XY_COMPONENTS, INTENSITY_DIRECTION };

class TimeSeries {
private:
	int timeStamp;
	double value1;
    double value2;
    uint objectId;
    QString objectType;
    TimeSeriesType timeSeriesType;
public:
	int getTimeStamp() const;
	void setTimeStamp(int timeStamp);
	QDateTime toDateTime() const;
	double getValue1() const;
	void setValue1(double value1);
    double getValue2() const;
    void setValue2(double value2);
    uint getObjectId() const;
    void setObjectId(uint objectId);
    QString getObjectType() const;
    void setObjectType(const QString &objectType);
    TimeSeriesType getTimeSeriesType() const;
    void setTimeSeriesType(const TimeSeriesType &timeSeriesType);
    SimulationDataType::TimeSeries toSimulationDataType() const;
    static TimeSeriesType mapStringToEnumType(const QString &typeStr);
};

#endif // TIME_SERIES_H