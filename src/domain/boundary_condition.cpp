#include <domain/boundary_condition.h>

BoundaryCondition::BoundaryCondition() :
    id(0),
    type(BoundaryConditionType::WATER_LEVEL),
    function(BoundaryConditionFunction::CONSTANT),
    verticalIntegratedOutflow(true),
    timeSeriesChanged(false)
{}

BoundaryCondition::~BoundaryCondition() {
    for (TimeSeries *timeSeries : timeSeriesList) {
        delete timeSeries;
    }
}

uint BoundaryCondition::getId() const {
    return id;
}

void BoundaryCondition::setId(const uint &id) {
    this->id = id;
}

bool BoundaryCondition::isPersisted() const {
    return id != 0;
}

QString BoundaryCondition::getName() const {
    return name;
}

void BoundaryCondition::setName(const QString &name) {
    this->name = name;
}

BoundaryConditionType BoundaryCondition::getType() const {
    return type;
}

void BoundaryCondition::setType(const BoundaryConditionType &type) {
    this->type = type;
}

BoundaryConditionFunction BoundaryCondition::getFunction() const {
    return function;
}

void BoundaryCondition::setFunction(const BoundaryConditionFunction &function) {
    this->function = function;
}

double BoundaryCondition::getConstantValue() const {
    return constantValue;
}

void BoundaryCondition::setConstantValue(double constantValue) {
    this->constantValue = constantValue;
}

InputModule BoundaryCondition::getInputModule() const {
    return inputModule;
}

void BoundaryCondition::setInputModule(const InputModule &inputModule) {
    this->inputModule = inputModule;
}

QList<TimeSeries*> BoundaryCondition::getTimeSeriesList() const {
    return timeSeriesList;
}

QList<TimeSeries*>* BoundaryCondition::getTimeSeriesListPointer() {
    return &timeSeriesList;
}

void BoundaryCondition::setTimeSeriesList(const QList<TimeSeries*> &timeSeriesList) {
    this->timeSeriesList = timeSeriesList;
}

bool BoundaryCondition::addTimeSeries(TimeSeries *timeSeries) {
    if (timeSeriesList.contains(timeSeries)) {
        return false;
    }

    timeSeriesList.append(timeSeries);

    return true;
}

bool BoundaryCondition::isTimeSeriesChanged() const {
    return timeSeriesChanged;
}

void BoundaryCondition::setTimeSeriesChanged(bool timeSeriesChanged) {
    this->timeSeriesChanged = timeSeriesChanged;
}

QString BoundaryCondition::getObjectTypeLabel() const {
    if (this->type == BoundaryConditionType::WATER_FLOW || this->type == BoundaryConditionType::NORMAL_DEPTH) {
        return "Edges";
    }
    
    if (this->type == BoundaryConditionType::WATER_LEVEL) {
        return "Cells";
    }
    
    return "Undefined";
}

QString BoundaryCondition::getTypeLabel() const {
    if (this->type == BoundaryConditionType::WATER_LEVEL) {
        return "Water Level";
    }
    
    if (this->type == BoundaryConditionType::WATER_FLOW) {
        return "Water Flow";
    }
    
    if (this->type == BoundaryConditionType::NORMAL_DEPTH) {
        return "Normal Depth";
    }
    
    return "Undefined";
}

QString BoundaryCondition::getFunctionLabel() const {
    return function == BoundaryConditionFunction::CONSTANT ? "Constant" : "Time Series";
}

bool BoundaryCondition::useVerticalIntegratedOutflow() const {
    return verticalIntegratedOutflow;
}

void BoundaryCondition::setVerticalIntegratedOutflow(bool verticalIntegratedOutflow) {
    this->verticalIntegratedOutflow = verticalIntegratedOutflow;
}

double BoundaryCondition::getMinimumElevation() const {
    return minimumElevation;
}

void BoundaryCondition::setMinimumElevation(const double &minimumElevation) {
    this->minimumElevation = minimumElevation;
}

double BoundaryCondition::getMaximumElevation() const {
    return maximumElevation;
}

void BoundaryCondition::setMaximumElevation(const double &maximumElevation) {
    this->maximumElevation = maximumElevation;
}
