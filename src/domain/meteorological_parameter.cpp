#include "include/domain/meteorological_parameter.h"

MeteorologicalParameter::MeteorologicalParameter() : id(0) {}

MeteorologicalParameter::MeteorologicalParameter(const QString &name, const QString &unit, double constantValue) :
    id(0), name(name), unit(unit), function(MeteorologicalParameterFunction::CONSTANT), constantValue(constantValue),
    useXYComponent(false), xComponent(0), yComponent(0), intensity(0), direction(0)
{}

uint MeteorologicalParameter::getId() const {
    return id;
}

void MeteorologicalParameter::setId(uint id) {
    if (!isPersisted()) {
        this->id = id;
    }
}

bool MeteorologicalParameter::isPersisted() const {
    return id != 0;
}

QString MeteorologicalParameter::getName() const {
    return name;
}

void MeteorologicalParameter::setName(const QString &name) {
    this->name = name;
}

QString MeteorologicalParameter::getUnit() const {
    return unit;
}

void MeteorologicalParameter::setUnit(const QString &unit) {
    this->unit = unit;
}

MeteorologicalParameterFunction MeteorologicalParameter::getFunction() const {
    return function;
}

QString MeteorologicalParameter::getFunctionStr() const {
    if (function == MeteorologicalParameterFunction::CONSTANT) {
        return "Constant";
    }
    if (function == MeteorologicalParameterFunction::TIME_SERIES) {
        return "Time Series";
    }

    return "Modeling";
}

void MeteorologicalParameter::setFunction(const MeteorologicalParameterFunction &function) {
    this->function = function;
}

void MeteorologicalParameter::setFunction(const QString &functionStr) {
    if (functionStr == "Constant") {
        this->function = MeteorologicalParameterFunction::CONSTANT;
    } else if (functionStr == "Time Series") {
        this->function = MeteorologicalParameterFunction::TIME_SERIES;
    } else {
        this->function = MeteorologicalParameterFunction::MODELING;
    }
}

double MeteorologicalParameter::getConstantValue() const {
    return constantValue;
}

void MeteorologicalParameter::setConstantValue(double constantValue) {
    this->constantValue = constantValue;
}

QList<TimeSeries*> MeteorologicalParameter::getTimeSeriesList() const {
    return timeSeriesList;
}

TimeSeries* MeteorologicalParameter::getTimeSeries(uint id) const {
    for (TimeSeries *timeSeries : timeSeriesList) {
        if (timeSeries->getId() == id) {
            return timeSeries;
        }
    }
    
    return nullptr;
}

void MeteorologicalParameter::setTimeSeriesList(const QList<TimeSeries*> timeSeriesList) {
    this->timeSeriesList = timeSeriesList;
}

void MeteorologicalParameter::addTimeSeries(TimeSeries *timeSeries) {
    timeSeriesList.append(timeSeries);
}

double MeteorologicalParameter::getXComponent() const {
    return xComponent;
}

void MeteorologicalParameter::setXComponent(double xComponent) {
    this->xComponent = xComponent;
}

double MeteorologicalParameter::getYComponent() const {
    return yComponent;
}

void MeteorologicalParameter::setYComponent(double yComponent) {
    this->yComponent = yComponent;
}

double MeteorologicalParameter::getIntensity() const {
    return intensity;
}

void MeteorologicalParameter::setIntensity(double intensity) {
    this->intensity = intensity;
}

double MeteorologicalParameter::getDirection() const {
    return direction;
}

void MeteorologicalParameter::setDirection(double direction) {
    this->direction = direction;
}

bool MeteorologicalParameter::getUseXYComponent() const {
    return useXYComponent;
}

void MeteorologicalParameter::setUseXYComponent(bool useXYComponent) {
    this->useXYComponent = useXYComponent;
}

QList<MeteorologicalParameter*> MeteorologicalParameter::createDefaultParameters() {
    return {
        new MeteorologicalParameter("Atmospheric Pressure", "atm", 1),
        new MeteorologicalParameter("Air Temperature", "Celsius", 20),
        new MeteorologicalParameter("Relative Humidity", "%", 100),
        new MeteorologicalParameter("Solar Radiation", "W/m\u00b2", 0),
        new MeteorologicalParameter("Wind", "m/s", 0),
        new MeteorologicalParameter("Precipitation", "mm/day", 0),
        new MeteorologicalParameter("Evaporation", "mm/day", 0)
    };
}

SimulationDataType::MeteorologicalParameter MeteorologicalParameter::toSimulationDataType() const {
    SimulationDataType::MeteorologicalParameter parameter;
    std::string nameStdString = this->name.toStdString();
    
    parameter.nameLength = this->name.length();
    parameter.name = new char[this->name.length()];
    strncpy(parameter.name, nameStdString.c_str(), this->name.length());
    parameter.functionMet = (int) this->function;
    parameter.constantValue = this->constantValue;
    parameter.useXYComponent = this->useXYComponent;
    parameter.xComponent = this->xComponent;
    parameter.yComponent = this->yComponent;
    parameter.intensity = this->intensity;
    parameter.direction = this->direction;
    parameter.timeSizeListLength = this->timeSeriesList.size();
    parameter.timeSeriesList = new SimulationDataType::TimeSeries[this->timeSeriesList.size()];
    
    for (int i = 0; i < this->timeSeriesList.size(); i++) {
        parameter.timeSeriesList[i] = this->timeSeriesList[i]->toSimulationDataType();
    }
    
    return parameter;
}