#ifndef METEOROLOGICAL_PARAMETER_H
#define METEOROLOGICAL_PARAMETER_H

#include "time_series.h"

#include <QList>
#include <QMetaType>

enum class MeteorologicalParameterFunction { CONSTANT = 1, TIME_SERIES, MODELING };

class MeteorologicalParameter {
protected:
    uint id;
    QString name;
    QString unit;
    MeteorologicalParameterFunction function;
    double constantValue;
    QList<TimeSeries*> timeSeriesList;
    
    // Wind parameter
    double xComponent;
    double yComponent;
    double intensity;
    double direction;
    bool useXYComponent;
public:
    MeteorologicalParameter();
    MeteorologicalParameter(const QString &name, const QString &unit, double defaultConstantValue);
    uint getId() const;
    void setId(uint id);
    bool isPersisted() const;
    QString getName() const;
    void setName(const QString &name);
    QString getUnit() const;
    void setUnit(const QString &unit);
    MeteorologicalParameterFunction getFunction() const;
    void setFunction(const MeteorologicalParameterFunction &function);
    void setFunction(const QString &functionStr);
    QString getFunctionStr() const;
    double getConstantValue() const;
    void setConstantValue(double constantValue);
    QList<TimeSeries*> getTimeSeriesList() const;
    QList<TimeSeries*>* getTimeSeriesListPointer();
    TimeSeries* getTimeSeries(uint id) const;
    void setTimeSeriesList(const QList<TimeSeries*> timeSeriesList);
    void addTimeSeries(TimeSeries *timeSeries);
    double getXComponent() const;
    void setXComponent(double xComponent);
    double getYComponent() const;
    void setYComponent(double yComponent);
    double getIntensity() const;
    void setIntensity(double intensity);
    double getDirection() const;
    void setDirection(double direction);
    bool getUseXYComponent() const;
    void setUseXYComponent(bool useXYComponent);
    static QList<MeteorologicalParameter*> createDefaultParameters();
    SimulationDataType::MeteorologicalParameter toSimulationDataType() const;
};

Q_DECLARE_METATYPE(MeteorologicalParameter*)

#endif // METEOROLOGICAL_PARAMETER_H
