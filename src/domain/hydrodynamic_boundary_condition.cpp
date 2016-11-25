#include <domain/hydrodynamic_boundary_condition.h>

#include <vtkProperty.h>

HydrodynamicBoundaryCondition::HydrodynamicBoundaryCondition() :
    cellColor("#FF0000"),
    selectionActor(vtkSmartPointer<vtkActor>::New()),
    labelsActor(vtkSmartPointer<vtkActor2D>::New())
{
    selectionActor->GetProperty()->LightingOff();
}

QSet<vtkIdType> HydrodynamicBoundaryCondition::getObjectIds() const {
    return objectIds;
}

vtkSmartPointer<vtkIdTypeArray> HydrodynamicBoundaryCondition::getVTKObjectIds() const {
    vtkSmartPointer<vtkIdTypeArray> vtkObjectIds = vtkSmartPointer<vtkIdTypeArray>::New();
    QByteArray arrayName = getVTKObjectsArrayName().toLocal8Bit();
    int i = 0;
    
    vtkObjectIds->SetName(arrayName.constData());
    vtkObjectIds->SetNumberOfComponents(1);
    vtkObjectIds->SetNumberOfTuples(objectIds.size());
    
    for (vtkIdType objectId : this->objectIds) {
        vtkObjectIds->SetTuple1(i++, objectId);
    }
    
    return vtkObjectIds;
}

QString HydrodynamicBoundaryCondition::getVTKObjectsArrayName() const {
    return type == BoundaryConditionType::WATER_LEVEL ? "cellIds" : "edgeIds";
}

void HydrodynamicBoundaryCondition::setObjectIds(const QSet<vtkIdType> &objectIds) {
    this->objectIds = objectIds;
}

void HydrodynamicBoundaryCondition::setObjectIds(vtkIdTypeArray* objectIds) {
    for (vtkIdType i = 0; i < objectIds->GetNumberOfTuples(); i++) {
        vtkIdType objectId = objectIds->GetTuple1(i);
        
        if (this->objectIds.contains(objectId)) {
            this->objectIds.remove(objectId);
        } else {
            this->objectIds.insert(objectId);
        }
    }
    
    emit objectIdsChanged();
}

void HydrodynamicBoundaryCondition::setObjectIds(const QString &objectIdsStr) {
    QStringList objectIdsStrList = objectIdsStr.split(",");

    for (QString objectId : objectIdsStrList) {
        this->objectIds.insert(objectId.toLongLong());
    }
    
    emit objectIdsChanged();
}

void HydrodynamicBoundaryCondition::addObjectId(const vtkIdType &objectId) {
    if (objectIds.contains(objectId)) {
        objectIds.remove(objectId);
    } else {
        objectIds.insert(objectId);
    }
    
    emit objectIdsChanged();
}

void HydrodynamicBoundaryCondition::removeObjectId(const vtkIdType &objectId) {
    objectIds.remove(objectId);
    emit objectIdsChanged();
}

void HydrodynamicBoundaryCondition::clearObjectIds() {
    objectIds.clear();
    emit objectIdsChanged();
}

QString HydrodynamicBoundaryCondition::getObjectIdsStr() const {
    QStringList objectIdsStr;

    for (vtkIdType objectId : this->objectIds) {
        objectIdsStr << QString::number(objectId);
    }

    return objectIdsStr.join(",");
}

QString HydrodynamicBoundaryCondition::getCellColor() const {
    return cellColor;
}

void HydrodynamicBoundaryCondition::setCellColor(const QString &cellColor) {
    this->cellColor = cellColor;
}

vtkSmartPointer<vtkActor> HydrodynamicBoundaryCondition::getSelectionActor() const {
    return selectionActor;
}

void HydrodynamicBoundaryCondition::setSelectionActor(vtkSmartPointer<vtkActor> selectionActor) {
    this->selectionActor = selectionActor;
}

vtkSmartPointer<vtkActor2D> HydrodynamicBoundaryCondition::getLabelsActor() const {
    return labelsActor;
}

void HydrodynamicBoundaryCondition::setLabelsActor(vtkSmartPointer<vtkActor2D> labelsActor) {
    this->labelsActor = labelsActor;
}

SimulationDataType::HydrodynamicBoundaryCondition HydrodynamicBoundaryCondition::toSimulationDataType(Mesh *mesh) const {
    SimulationDataType::HydrodynamicBoundaryCondition boundaryCondition;
    vtkSmartPointer<vtkPolyData> meshPolyData = mesh->getMeshPolyData();
    int cellCount = 0;
    
    boundaryCondition.conditionType = (int) this->type;
    boundaryCondition.cellsLength = this->objectIds.size();
    boundaryCondition.cells = new SimulationDataType::HydrodynamicBoundaryConditionCell[boundaryCondition.cellsLength];
    
    if (this->type == BoundaryConditionType::WATER_LEVEL) {
        for (vtkIdType objectId : objectIds) {
            boundaryCondition.cells[cellCount] = SimulationDataType::HydrodynamicBoundaryConditionCell();
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
            
            boundaryCondition.cells[cellCount] = SimulationDataType::HydrodynamicBoundaryConditionCell();
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
    boundaryCondition.minimumElevation = this->minimumElevation;
    boundaryCondition.maximumElevation = this->maximumElevation;
    
    return boundaryCondition;
}
