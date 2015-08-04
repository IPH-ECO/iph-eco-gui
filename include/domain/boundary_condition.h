#ifndef BOUNDARY_CONDITION_H
#define BOUNDARY_CONDITION_H

#include "include/domain/time_series.h"

#include <vtkIdTypeArray.h>
#include <QString>

enum class BoundaryConditionType { WATER_LEVEL = 1, WATER_FLOW };
enum class BoundaryConditionFunction { CONSTANT = 1, TIME_SERIES };
enum class InputModule { HYDRODYNAMIC = 1, WATER_QUALITY };

class BoundaryCondition {
private:
	uint id;
	BoundaryConditionType type;
	vtkIdTypeArray *objectIds;
	BoundaryConditionFunction function;
	double constantValue;
	InputModule inputModule;
	QList<TimeSeries*> timeSeriesList;

public:
	BoundaryCondition();
	~BoundaryCondition();

	uint getId() const;
	void setId(uint id);
	bool isPersisted() const;
	BoundaryConditionType getType() const;
    QString getTypeStr() const;
	void setType(const BoundaryConditionType &type);
	vtkIdTypeArray* getObjectIds() const;
	void setObjectIds(vtkIdTypeArray *objectIds);
	void setObjectIds(const QString &objectIdsStr);
	BoundaryConditionFunction getFunction() const;
    QString getFunctionStr() const;
	void setFunction(const BoundaryConditionFunction &function);
	double getConstantValue() const;
	void setConstantValue(double constantValue);
	InputModule getInputModule() const;
	void setInputModule(const InputModule &inputModule);
	QString getObjectIdsStr() const;
	QList<TimeSeries*> getTimeSeriesList() const;
	void setTimeSeriesList(const QList<TimeSeries*> &timeSeriesList);
	bool addTimeSeries(TimeSeries *timeSeries);
};

#endif // BOUNDARY_CONDITION_H