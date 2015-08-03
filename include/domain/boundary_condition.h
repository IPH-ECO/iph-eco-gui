#ifndef BOUNDARY_CONDITION_H
#define BOUNDARY_CONDITION_H

#include <vtkIdTypeArray.h>
#include <QString>

enum class BoundaryConditionType { WATER_LEVEL = 1, WATER_FLOW };
enum class InputModule { HYDRODYNAMIC = 1, WATER_QUALITY };

class BoundaryCondition {
private:
	uint id;
	BoundaryConditionType type;
	vtkIdTypeArray *objectIds;
	QString function;
	double constantValue;
	InputModule inputModule;

public:
	BoundaryCondition();
	~BoundaryCondition();

	uint getId() const;
	void setId(uint id);
	bool isPersisted() const;
	BoundaryConditionType getType() const;
	void setType(const BoundaryConditionType &type);
	vtkIdTypeArray* getObjectIds() const;
	void setObjectIds(vtkIdTypeArray *objectIds);
	void setObjectIds(const QString &objectIdsStr);
	QString getFunction() const;
	void setFunction(const QString &function);
	double getConstantValue() const;
	void setConstantValue(double constantValue);
	InputModule getInputModule() const;
	void setInputModule(const InputModule &inputModule);
	QString getObjectIdsStr() const;

	static BoundaryConditionType mapTypeFromString(const QString &typeStr);
    static QString mapStringFromType(const BoundaryConditionType &type);
};

#endif // BOUNDARY_CONDITION_H