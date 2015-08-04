#ifndef TIME_SERIES_H
#define TIME_SERIES_H

#include <QDateTime>

class TimeSeries {
private:
	uint id;
	QString timestamp;
	double value;
public:
	TimeSeries();
	uint getId() const;
	void setId(uint id);
	bool isPersisted() const;
	QString getTimestamp() const;
	void setTimestamp(const QString &timestamp);
	QDateTime toDateTime() const;
	double getValue() const;
	void setValue(double value);
};

#endif // TIME_SERIES_H