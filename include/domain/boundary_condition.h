#ifndef BOUNDARY_CONDITION_H
#define BOUNDARY_CONDITION_H

#include "include/domain/time_series.h"

#include <vtkSmartPointer.h>
#include <vtkIdTypeArray.h>
#include <vtkActor2D.h>
#include <vtkActor.h>
#include <QString>
#include <QSet>

enum class BoundaryConditionType { WATER_LEVEL = 1, WATER_FLOW };
enum class BoundaryConditionFunction { CONSTANT = 1, TIME_SERIES };
enum class InputModule { HYDRODYNAMIC = 1, WATER_QUALITY };

class BoundaryCondition {
private:
	uint id;
	BoundaryConditionType type;
	QSet<vtkIdType> objectIds;
	BoundaryConditionFunction function;
	double constantValue;
	InputModule inputModule;
	QList<TimeSeries*> timeSeriesList;
    QString cellColor;
    
    // Transient attributes
    vtkSmartPointer<vtkActor> selectionActor;
    vtkSmartPointer<vtkActor2D> labelsActor;

public:
	BoundaryCondition();

	uint getId() const;
	void setId(uint id);
	bool isPersisted() const;
	BoundaryConditionType getType() const;
    QString getTypeStr() const;
	void setType(const BoundaryConditionType &type);
	QSet<vtkIdType> getObjectIds() const;
    vtkSmartPointer<vtkIdTypeArray> getVtkObjectIds() const;
    void setObjectIds(const QSet<vtkIdType> &objectIds);
    void setObjectIds(vtkIdTypeArray* objectIds);
	void setObjectIds(const QString &objectIdsStr);
    void addObjectId(const vtkIdType &objectId);
    void removeObjectId(const vtkIdType &objectId);
    void emptyObjectIds();
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
    QString getCellColor() const;
    void setCellColor(const QString &cellColor);
    
    vtkSmartPointer<vtkActor> getSelectionActor() const;
    void setSelectionActor(vtkSmartPointer<vtkActor> selectionActor);
    vtkSmartPointer<vtkActor2D> getLabelsActor() const;
    void setLabelsActor(vtkSmartPointer<vtkActor2D> labelsActor);
};

#endif // BOUNDARY_CONDITION_H