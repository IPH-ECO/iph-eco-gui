#include <utility/time_series_chart_mouse_interactor.h>

#include <vtkCellData.h>
#include <vtkAreaPicker.h>
#include <vtkCellPicker.h>
#include <vtkExtractSelectedFrustum.h>

TimeSeriesChartMouseInteractor::TimeSeriesChartMouseInteractor() {}

void TimeSeriesChartMouseInteractor::OnLeftButtonUp() {
	vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
    
    if (pickerMode == PickerMode::MULTIPLE_CELL) {
        vtkSmartPointer<vtkExtractSelectedFrustum> extractor = vtkSmartPointer<vtkExtractSelectedFrustum>::New();
        extractor->PreserveTopologyOff();
        extractor->SetInputData(meshPolyData);
        extractor->SetFrustum(vtkAreaPicker::SafeDownCast(this->GetInteractor()->GetPicker())->GetFrustum());
        extractor->Update();
        
        vtkSmartPointer<vtkPolyData> selectionPolyData = vtkSmartPointer<vtkPolyData>::New();
        selectionPolyData->ShallowCopy(extractor->GetOutput());
        
        if (selectionPolyData->GetCellData()->HasArray("vtkOriginalCellIds")) {
            
        }
    }
}

void TimeSeriesChartMouseInteractor::pickCell() {
    if (pickerMode == PickerMode::INDIVIDUAL_CELL && lastCellId != -1) {
        
    }
}
