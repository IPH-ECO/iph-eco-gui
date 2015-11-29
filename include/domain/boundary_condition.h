#ifndef BOUNDARY_CONDITION_H
#define BOUNDARY_CONDITION_H

#include "simulation_data_type.h"
#include "time_series.h"
#include "mesh.h"

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
    bool verticalIntegratedOutflow;
    double quota;
    
    // Transient attributes
    vtkSmartPointer<vtkActor> selectionActor;
    vtkSmartPointer<vtkActor2D> labelsActor;
    bool timeSeriesChanged;

public:
	BoundaryCondition();

	uint getId() const;
	void setId(uint id);
	bool isPersisted() const;
	BoundaryConditionType getType() const;
    QString getTypeStr() const;
	void setType(const BoundaryConditionType &type);
	QSet<vtkIdType> getObjectIds() const;
    vtkSmartPointer<vtkIdTypeArray> getVTKObjectIds() const;
    QString getVTKObjectsArrayName() const;
    void setObjectIds(const QSet<vtkIdType> &objectIds);
    void setObjectIds(vtkIdTypeArray* objectIds);
	void setObjectIds(const QString &objectIdsStr);
    void addObjectId(const vtkIdType &objectId);
    void removeObjectId(const vtkIdType &objectId);
    void clearObjectIds();
	BoundaryConditionFunction getFunction() const;
    QString getFunctionStr() const;
	void setFunction(const BoundaryConditionFunction &function);
	double getConstantValue() const;
	void setConstantValue(double constantValue);
	InputModule getInputModule() const;
	void setInputModule(const InputModule &inputModule);
	QString getObjectIdsStr() const;
	QList<TimeSeries*> getTimeSeriesList() const;
    QList<TimeSeries*>* getTimeSeriesListPointer();
	void setTimeSeriesList(const QList<TimeSeries*> &timeSeriesList);
	bool addTimeSeries(TimeSeries *timeSeries);
    QString getCellColor() const;
    void setCellColor(const QString &cellColor);
    bool useVerticalIntegratedOutflow() const;
    void setVerticalIntegratedOutflow(bool verticalIntegratedOutflow);
    double getQuota() const;
    void setQuota(const double &quota);
    
    vtkSmartPointer<vtkActor> getSelectionActor() const;
    void setSelectionActor(vtkSmartPointer<vtkActor> selectionActor);
    vtkSmartPointer<vtkActor2D> getLabelsActor() const;
    void setLabelsActor(vtkSmartPointer<vtkActor2D> labelsActor);
    bool isTimeSeriesChanged() const;
    void setTimeSeriesChanged(bool timeSeriesChanged);
    
    SimulationDataType::BoundaryCondition toSimulationDataType(Mesh *mesh) const;
};

#endif // BOUNDARY_CONDITION_H