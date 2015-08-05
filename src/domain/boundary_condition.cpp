#include "include/domain/boundary_condition.h"

#include <QStringList>

BoundaryCondition::BoundaryCondition() : id(0) {
    objectIds = vtkIdTypeArray::New();
}

BoundaryCondition::~BoundaryCondition() {
	objectIds->Delete();
}

uint BoundaryCondition::getId() const {
	return id;
}

void BoundaryCondition::setId(uint id) {
	if (!isPersisted()) {
		this->id = id;
	}
}

bool BoundaryCondition::isPersisted() const {
	return id != 0;
}

BoundaryConditionType BoundaryCondition::getType() const {
	return type;
}

QString BoundaryCondition::getTypeStr() const {
    return type == BoundaryConditionType::WATER_LEVEL ? "Water Level" : "Water Flow";
}

void BoundaryCondition::setType(const BoundaryConditionType &type) {
	this->type = type;
}

vtkIdTypeArray* BoundaryCondition::getObjectIds() const {
	return objectIds;
}

void BoundaryCondition::setObjectIds(vtkIdTypeArray *objectIds) {
    this->objectIds->Delete();
	this->objectIds = objectIds;
}

void BoundaryCondition::setObjectIds(const QString &objectIdsStr) {
	QStringList objectIdsStrList = objectIdsStr.split(",");
	vtkIdType size = objectIdsStrList.size();

    objectIds->SetNumberOfTuples(size);

	for (vtkIdType i = 0; i < size; i++) {
		objectIds->SetTuple1(i, objectIdsStrList[i].toUInt());
	}
}

BoundaryConditionFunction BoundaryCondition::getFunction() const {
	return function;
}

QString BoundaryCondition::getFunctionStr() const {
    return function == BoundaryConditionFunction::CONSTANT ? "Constant" : "Time Series";
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

QString BoundaryCondition::getObjectIdsStr() const {
	QStringList objectIdsStr;

	for (vtkIdType i = 0; i < objectIds->GetNumberOfTuples(); i++) {
		objectIdsStr << QString::number(objectIds->GetTuple1(i));
	}

	return objectIdsStr.join(",");
}

QList<TimeSeries*> BoundaryCondition::getTimeSeriesList() const {
	return timeSeriesList;
}

void BoundaryCondition::setTimeSeriesList(const QList<TimeSeries*> &timeSeriesList) {
    for (int i = 0; i < this->timeSeriesList.size(); i++) {
        delete this->timeSeriesList[i];
    }
    this->timeSeriesList.clear();
    
	this->timeSeriesList = timeSeriesList;
}

bool BoundaryCondition::addTimeSeries(TimeSeries *timeSeries) {
	if (timeSeriesList.contains(timeSeries)) {
		return false;
	}

	timeSeriesList.append(timeSeries);

	return true;
}