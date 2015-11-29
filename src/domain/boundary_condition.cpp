#include "include/domain/boundary_condition.h"

#include <QStringList>
#include <vtkProperty.h>

BoundaryCondition::BoundaryCondition() :
    id(0),
    type(BoundaryConditionType::WATER_LEVEL),
    function(BoundaryConditionFunction::CONSTANT),
    cellColor("#FF0000"),
    selectionActor(vtkSmartPointer<vtkActor>::New()),
    labelsActor(vtkSmartPointer<vtkActor2D>::New()),
    timeSeriesChanged(false)
{
    selectionActor->GetProperty()->LightingOff();
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

QSet<vtkIdType> BoundaryCondition::getObjectIds() const {
	return objectIds;
}

vtkSmartPointer<vtkIdTypeArray> BoundaryCondition::getVTKObjectIds() const {
    vtkSmartPointer<vtkIdTypeArray> vtkObjectIds = vtkSmartPointer<vtkIdTypeArray>::New();
    std::string arrayName = getVTKObjectsArrayName().toStdString();
    int i = 0;
    
    vtkObjectIds->SetName(arrayName.c_str());
    vtkObjectIds->SetNumberOfComponents(1);
    vtkObjectIds->SetNumberOfTuples(objectIds.size());
    
    for (vtkIdType objectId : this->objectIds) {
        vtkObjectIds->SetTuple1(i++, objectId);
    }
    
    return vtkObjectIds;
}

QString BoundaryCondition::getVTKObjectsArrayName() const {
    return type == BoundaryConditionType::WATER_LEVEL ? "cellIds" : "edgeIds";
}

void BoundaryCondition::setObjectIds(const QSet<vtkIdType> &objectIds) {
	this->objectIds = objectIds;
}

void BoundaryCondition::setObjectIds(vtkIdTypeArray* objectIds) {
    for (vtkIdType i = 0; i < objectIds->GetNumberOfTuples(); i++) {
        vtkIdType objectId = objectIds->GetTuple1(i);
        
        if (this->objectIds.contains(objectId)) {
            this->objectIds.remove(objectId);
        } else {
            this->objectIds.insert(objectId);
        }
    }
}

void BoundaryCondition::setObjectIds(const QString &objectIdsStr) {
	QStringList objectIdsStrList = objectIdsStr.split(",");

    for (QString objectId : objectIdsStrList) {
        this->objectIds.insert(objectId.toLongLong());
	}
}

void BoundaryCondition::addObjectId(const vtkIdType &objectId) {
    if (objectIds.contains(objectId)) {
        objectIds.remove(objectId);
    } else {
        objectIds.insert(objectId);
    }
}

void BoundaryCondition::removeObjectId(const vtkIdType &objectId) {
    objectIds.remove(objectId);
}

void BoundaryCondition::clearObjectIds() {
    objectIds.clear();
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

    for (vtkIdType objectId : this->objectIds) {
		objectIdsStr << QString::number(objectId);
	}

	return objectIdsStr.join(",");
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

QString BoundaryCondition::getCellColor() const {
    return cellColor;
}

void BoundaryCondition::setCellColor(const QString &cellColor) {
    this->cellColor = cellColor;
}

bool BoundaryCondition::useVerticalIntegratedOutflow() const {
    return verticalIntegratedOutflow;
}

void BoundaryCondition::setVerticalIntegratedOutflow(bool verticalIntegratedOutflow) {
    this->verticalIntegratedOutflow = verticalIntegratedOutflow;
}

double BoundaryCondition::getQuota() const {
    return quota;
}

void BoundaryCondition::setQuota(const double &quota) {
    this->quota = quota;
}

vtkSmartPointer<vtkActor> BoundaryCondition::getSelectionActor() const {
    return selectionActor;
}

void BoundaryCondition::setSelectionActor(vtkSmartPointer<vtkActor> selectionActor) {
    this->selectionActor = selectionActor;
}

vtkSmartPointer<vtkActor2D> BoundaryCondition::getLabelsActor() const {
    return labelsActor;
}

void BoundaryCondition::setLabelsActor(vtkSmartPointer<vtkActor2D> labelsActor) {
    this->labelsActor = labelsActor;
}

bool BoundaryCondition::isTimeSeriesChanged() const {
    return timeSeriesChanged;
}

void BoundaryCondition::setTimeSeriesChanged(bool timeSeriesChanged) {
    this->timeSeriesChanged = timeSeriesChanged;
}

SimulationDataType::BoundaryCondition BoundaryCondition::toSimulationDataType(Mesh *mesh) const {
    SimulationDataType::BoundaryCondition boundaryCondition;
    vtkSmartPointer<vtkPolyData> meshPolyData = mesh->getMeshPolyData();
    int cellCount = 0;
    
    boundaryCondition.conditionType = (int) this->type;
    boundaryCondition.cellsLength = this->objectIds.size();
	boundaryCondition.cells = new SimulationDataType::BoundaryConditionCell[boundaryCondition.cellsLength];
    
    if (this->type == BoundaryConditionType::WATER_LEVEL) {
        for (vtkIdType objectId : objectIds) {
			boundaryCondition.cells[cellCount] = SimulationDataType::BoundaryConditionCell();
			boundaryCondition.cells[cellCount].cellId = objectId;
			cellCount++;
        }
    } else {
        vtkSmartPointer<vtkPolyData> boundaryPolyData = mesh->getBoundaryPolyData();
        
        for (vtkIdType objectId : objectIds) {
            vtkSmartPointer<vtkCell> edge = boundaryPolyData->GetCell(objectId);
            double edgeA[3], edgeB[3];
            
            edge->GetPoints()->GetPoint(0, edgeA);
            edge->GetPoints()->GetPoint(1, edgeB);
            
            vtkIdType meshPointAId = meshPolyData->FindPoint(edgeA);
            vtkIdType meshPointBId = meshPolyData->FindPoint(edgeB);
            vtkSmartPointer<vtkIdList> cellsPointA = vtkSmartPointer<vtkIdList>::New();
            vtkSmartPointer<vtkIdList> cellsPointB = vtkSmartPointer<vtkIdList>::New();
            
            meshPolyData->GetPointCells(meshPointAId, cellsPointA);
            meshPolyData->GetPointCells(meshPointBId, cellsPointB);
            
            vtkIdType foundCellId = -1;
            
            for (vtkIdType i = 0; i < cellsPointA->GetNumberOfIds() && foundCellId == -1; i++) {
                for (vtkIdType j = 0; j < cellsPointB->GetNumberOfIds() && foundCellId == -1; j++) {
                    if (cellsPointA->GetId(i) == cellsPointB->GetId(j)) {
                        foundCellId = i;
                    }
                }
            }
            
			boundaryCondition.cells[cellCount] = SimulationDataType::BoundaryConditionCell();
			boundaryCondition.cells[cellCount].cellId = cellsPointA->GetId(foundCellId);
			boundaryCondition.cells[cellCount].verticeIds[0] = meshPolyData->FindPoint(edgeA);
			boundaryCondition.cells[cellCount].verticeIds[1] = meshPolyData->FindPoint(edgeB);

			cellCount++;
        }
    }
    
    boundaryCondition.conditionFunction = (int) this->function;
    boundaryCondition.constantValue = this->constantValue;
    boundaryCondition.timeSeriesListSize = this->timeSeriesList.size();
    boundaryCondition.timeSeriesList = new SimulationDataType::TimeSeries[boundaryCondition.timeSeriesListSize];
    
    for (vtkIdType i = 0; i < this->timeSeriesList.size(); i++) {
        boundaryCondition.timeSeriesList[i] = this->timeSeriesList[i]->toSimulationDataType();
    }
    
    boundaryCondition.verticalIntegratedOutflow = this->verticalIntegratedOutflow;
    boundaryCondition.quota = this->quota;
    
    return boundaryCondition;
}