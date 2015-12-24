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
    if (pickerMode == PickerMode::INDIVIDUAL_CELL && lastCellId != -1) {
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
        
        cellIdArray->InsertNextValue(lastCellId);
        
        vtkSmartPointer<vtkUnsignedCharArray> cellData = vtkSmartPointer<vtkUnsignedCharArray>::New();
        cellData->SetNumberOfComponents(3);
        cellData->InsertNextTuple3(vtkMath::Random(0.0, 255.0), vtkMath::Random(0.0, 255.0), vtkMath::Random(0.0, 255.0));
        
        vtkSmartPointer<vtkIdTypeArray> selectionArray = vtkSmartPointer<vtkIdTypeArray>::New();
        selectionArray->InsertNextValue(lastCellId);
        
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
        this->cellActors.insert(lastCellId, cellActor);
        this->GetDefaultRenderer()->AddActor(cellActor);
        
        this->GetDefaultRenderer()->GetRenderWindow()->Render();
    }
}

vtkSmartPointer<vtkIdTypeArray> TimeSeriesChartMouseInteractor::getCellIdArray(const QString &layerKey) const {
    return cellIdMap.value(layerKey);
}

void TimeSeriesChartMouseInteractor::deactivatePicker(const QString &layerKey) {
    vtkSmartPointer<vtkIdTypeArray> cellIds = this->cellIdMap.value(layerKey);
    
    if (cellIds) {
        for (vtkIdType i = 0; i < cellIds->GetNumberOfTuples(); i++) {
            this->GetDefaultRenderer()->RemoveActor(this->cellActors.value(i));
        }
    }
    
    this->cellIdMap.remove(layerKey);
    
    MeshMouseInteractor::deactivatePicker();
}