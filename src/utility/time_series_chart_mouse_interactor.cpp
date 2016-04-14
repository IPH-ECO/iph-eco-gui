#include <utility/time_series_chart_mouse_interactor.h>

#include <time.h>
#include <vtkMath.h>
#include <vtkProperty.h>
#include <vtkCellData.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkDataSetMapper.h>
#include <vtkUnsignedCharArray.h>
#include <vtkExtractSelectedIds.h>

TimeSeriesChartMouseInteractor::TimeSeriesChartMouseInteractor() {
    vtkMath::RandomSeed(time(NULL));
}

void TimeSeriesChartMouseInteractor::pickCell(vtkSmartPointer<vtkUnstructuredGrid> layerGrid, const QString &layerKey) {
    if (lastCellId != -1 && (pickerMode == PickerMode::EACH_CELL || pickerMode == PickerMode::ONE_CELL)) {
        vtkSmartPointer<vtkIdTypeArray> cellIdArray = cellIdMap.value(layerKey);
        
        if (cellIdMap.contains(layerKey)) {
            for (vtkIdType i = 0; i < cellIdArray->GetNumberOfTuples(); i++) {
                if (cellIdArray->GetValue(i) == lastCellId) {
                    cellIdArray->RemoveTuple(i);
                    this->GetDefaultRenderer()->RemoveActor(cellActors.value(lastCellId));
                    this->GetDefaultRenderer()->Render();
                    this->cellActors.remove(lastCellId);
                    return;
                }
            }
        } else {
            cellIdArray = vtkSmartPointer<vtkIdTypeArray>::New();
            cellIdMap.insert(layerKey, cellIdArray);
        }
        
        if (pickerMode == PickerMode::EACH_CELL || (pickerMode == PickerMode::ONE_CELL && cellIdArray->GetNumberOfTuples() == 0)) {
            cellIdArray->InsertNextValue(lastCellId);
            renderCellId(lastCellId, layerGrid);
            this->GetDefaultRenderer()->GetRenderWindow()->Render();
        }
    }
}

void TimeSeriesChartMouseInteractor::renderCellsIds(const QString &layerKey, vtkSmartPointer<vtkUnstructuredGrid> layerGrid, vtkSmartPointer<vtkIdTypeArray> cellsIds) {
    vtkSmartPointer<vtkIdTypeArray> existingCellsIds = cellIdMap.value(layerKey);
    
    if (existingCellsIds) {
        for (vtkIdType i = 0; i < cellsIds->GetNumberOfTuples(); i++) {
            bool insertTuple = true;
            
            for (vtkIdType j = 0; j < existingCellsIds->GetNumberOfTuples(); j++) {
                if (cellsIds->GetValue(i) == existingCellsIds->GetValue(j)) {
                    insertTuple = false;
                    break;
                }
            }
            
            if (insertTuple) {
                existingCellsIds->InsertNextTuple1(cellsIds->GetValue(i));
            }
        }
    } else {
        cellIdMap.insert(layerKey, cellsIds);
    }
    
    for (vtkIdType i = 0; i < cellsIds->GetNumberOfTuples(); i++) {
        renderCellId(cellsIds->GetValue(i), layerGrid);
    }
    
    this->GetDefaultRenderer()->GetRenderWindow()->Render();
}

vtkSmartPointer<vtkIdTypeArray> TimeSeriesChartMouseInteractor::getCellIdArray(const QString &layerKey) const {
    return cellIdMap.value(layerKey);
}

void TimeSeriesChartMouseInteractor::removePickedCells(const QString &layerKey) {
    if (layerKey.isEmpty()) {
        for (vtkSmartPointer<vtkIdTypeArray> cellIds : cellIdMap.values()) {
            for (vtkIdType i = 0; i < cellIds->GetNumberOfTuples(); i++) {
                this->GetDefaultRenderer()->RemoveActor(cellActors.value(cellIds->GetValue(i)));
            }
        }
        
        cellActors.clear();
        cellIdMap.clear();
    } else {
        vtkSmartPointer<vtkIdTypeArray> cellIds = cellIdMap.value(layerKey);
        
        if (cellIds) {
            for (vtkIdType i = 0; i < cellIds->GetNumberOfTuples(); i++) {
                vtkIdType cellId = cellIds->GetValue(i);
                vtkSmartPointer<vtkActor> actor = cellActors.value(cellId);
                this->GetDefaultRenderer()->RemoveActor(actor);
                cellActors.remove(cellId);
            }
            
            cellIdMap.remove(layerKey);
        }
    }
    
    this->GetDefaultRenderer()->GetRenderWindow()->Render();
}

void TimeSeriesChartMouseInteractor::getCellColor(const vtkIdType &cellId, double color[3]) const {
    cellActors.value(cellId)->GetMapper()->GetInput()->GetCellData()->GetScalars()->GetTuple(0, color);
}

void TimeSeriesChartMouseInteractor::renderCellId(const vtkIdType &cellId, vtkSmartPointer<vtkUnstructuredGrid> layerGrid) {
    if (cellActors.contains(cellId)) {
        return;
    }
    
    vtkSmartPointer<vtkUnsignedCharArray> cellData = vtkSmartPointer<vtkUnsignedCharArray>::New();
    cellData->SetNumberOfComponents(3);
    cellData->SetNumberOfTuples(1);
    cellData->InsertTuple3(0, vtkMath::Random(0.0, 255.0), vtkMath::Random(0.0, 255.0), vtkMath::Random(0.0, 255.0));
    
    vtkSmartPointer<vtkIdTypeArray> selectionArray = vtkSmartPointer<vtkIdTypeArray>::New();
    selectionArray->InsertNextValue(cellId);
    
    vtkSmartPointer<vtkSelectionNode> selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
    selectionNode->SetFieldType(vtkSelectionNode::CELL);
    selectionNode->SetContentType(vtkSelectionNode::INDICES);
    selectionNode->SetSelectionList(selectionArray);
    
    vtkSmartPointer<vtkSelection> selection = vtkSmartPointer<vtkSelection>::New();
    selection->AddNode(selectionNode);
    
    vtkSmartPointer<vtkExtractSelectedIds> extractSelection = vtkSmartPointer<vtkExtractSelectedIds>::New();
    extractSelection->SetInputData(0, layerGrid);
    extractSelection->SetInputData(1, selection);
    extractSelection->Update();
    
    vtkSmartPointer<vtkUnstructuredGrid> selectionUnstructuredGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    selectionUnstructuredGrid->DeepCopy(extractSelection->GetOutput());
    selectionUnstructuredGrid->GetCellData()->SetScalars(cellData);
    
    vtkSmartPointer<vtkDataSetMapper> cellSelectionMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    cellSelectionMapper->SetInputData(selectionUnstructuredGrid);
    
    vtkSmartPointer<vtkActor> cellActor = vtkSmartPointer<vtkActor>::New();
    cellActor->SetMapper(cellSelectionMapper);
    cellActor->GetProperty()->LightingOff();
    cellActor->PickableOff();
    this->cellActors.insert(cellId, cellActor);
    this->GetDefaultRenderer()->AddActor(cellActor);
}