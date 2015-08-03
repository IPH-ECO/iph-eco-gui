#include "include/domain/boundary_condition.h"

#include <QStringList>

BoundaryCondition::BoundaryCondition() : id(0) {}

BoundaryCondition::~BoundaryCondition() {
	objectIds->Delete();
}

uint BoundaryCondition::getId() const {
	return id;
}

void BoundaryCondition::setId(uint id) {
	this->id = id;
}

bool BoundaryCondition::isPersisted() const {
	return id != 0;
}

BoundaryConditionType BoundaryCondition::getType() const {
	return type;
}

void BoundaryCondition::setType(const BoundaryConditionType &type) {
	this->type = type;
}

vtkIdTypeArray* BoundaryCondition::getObjectIds() const {
	return objectIds;
}

void BoundaryCondition::setObjectIds(vtkIdTypeArray *objectIds) {
	this->objectIds = objectIds;
}

void BoundaryCondition::setObjectIds(const QString &objectIdsStr) {
	QStringList objectIdsStrList = objectIdsStr.split(",");
	vtkIdType size = objectIdsStrList.size();

    objectIds = vtkIdTypeArray::New();
	objectIds->SetNumberOfTuples(size);

	for (vtkIdType i = 0; i < size; i++) {
		objectIds->SetTuple1(i, objectIdsStrList[i].toUInt());
	}
}

QString BoundaryCondition::getFunction() const {
	return function;
}

void BoundaryCondition::setFunction(const QString &function) {
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

BoundaryConditionType BoundaryCondition::mapTypeFromString(const QString &typeStr) {
	if (typeStr == "Water Level") {
		return BoundaryConditionType::WATER_LEVEL;
	}

	return BoundaryConditionType::WATER_FLOW;
}

QString BoundaryCondition::mapStringFromType(const BoundaryConditionType &type) {
	if (type == BoundaryConditionType::WATER_LEVEL) {
		return "Water Level";
	}

	return "Water Flow";
}