#include <domain/boundary_condition.h>

BoundaryCondition::BoundaryCondition() :
    id(0),
    type(BoundaryConditionType::WATER_LEVEL),
    function(BoundaryConditionFunction::CONSTANT),
    verticallyIntegrated(true),
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

bool BoundaryCondition::isVerticallyIntegrated() const {
    return verticallyIntegrated;
}

void BoundaryCondition::setVerticallyIntegrated(const bool &verticallyIntegrated) {
    this->verticallyIntegrated = verticallyIntegrated;
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

QSet<NonVerticallyIntegratedRange*> BoundaryCondition::getNonVerticallyIntegratedRanges() const {
    return nonVerticallyIntegratedRanges;
}

void BoundaryCondition::setNonVerticallyIntegratedRanges(const QSet<NonVerticallyIntegratedRange*> &nonVerticallyIntegratedRanges) {
    this->nonVerticallyIntegratedRanges = nonVerticallyIntegratedRanges;
}

void BoundaryCondition::addNonVerticallyIntegratedRange(NonVerticallyIntegratedRange *verticallyIntegratedRange) {
    nonVerticallyIntegratedRanges.insert(verticallyIntegratedRange);
}

void BoundaryCondition::removeNonVerticallyIntegratedRange(NonVerticallyIntegratedRange *verticallyIntegratedRange) {
    nonVerticallyIntegratedRanges.remove(verticallyIntegratedRange);
}

bool BoundaryCondition::isTimeSeriesChanged() const {
    return timeSeriesChanged;
}

void BoundaryCondition::setTimeSeriesChanged(const bool &timeSeriesChanged) {
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
