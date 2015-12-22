#include <utility/time_series_chart_mouse_interactor.h>

#include <vtkCellData.h>
#include <vtkAreaPicker.h>
#include <vtkCellPicker.h>
#include <vtkExtractSelectedFrustum.h>

void TimeSeriesChartMouseInteractor::setLayerKey(const QString &layerKey) {
    this->layerKey = layerKey;
    
    if (!cellIdMap.contains(layerKey)) {
        cellIdMap.insert(layerKey, vtkSmartPointer<vtkIdTypeArray>::New());
    }
}

void TimeSeriesChartMouseInteractor::pickCell() {
    if (pickerMode == PickerMode::INDIVIDUAL_CELL && lastCellId != -1) {
        vtkSmartPointer<vtkIdTypeArray> cellIds = cellIdMap.value(layerKey);
        
        if (cellIds) {
            
        }
    }
}
