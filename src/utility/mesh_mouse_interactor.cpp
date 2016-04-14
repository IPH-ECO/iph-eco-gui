#include <utility/mesh_mouse_interactor.h>
#include <ui/main_window.h>

#include <QApplication>
#include <vtkCellPicker.h>
#include <vtkWorldPointPicker.h>

MeshMouseInteractor::MeshMouseInteractor() : pickerMode(PickerMode::NO_PICKER), lastCellId(-1) {}

void MeshMouseInteractor::OnLeftButtonDown() {
    int *clickPosition = this->GetInteractor()->GetEventPosition();
    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    vtkSmartPointer<vtkWorldPointPicker> worldPicker = vtkSmartPointer<vtkWorldPointPicker>::New();
    
    picker->SetTolerance(0.0005);
    picker->Pick(clickPosition[0], clickPosition[1], 0, this->GetDefaultRenderer());
    
    worldPicker->Pick(clickPosition[0], clickPosition[1], 0, this->GetDefaultRenderer());
    worldPicker->GetPickPosition(worldCoordinate);
    
    lastCellId = picker->GetCellId();
    
    vtkInteractorStyleRubberBandPick::OnLeftButtonDown();
}

void MeshMouseInteractor::OnMouseMove() {
    vtkSmartPointer<vtkWorldPointPicker> picker = vtkSmartPointer<vtkWorldPointPicker>::New();
    int *mousePosition = this->Interactor->GetEventPosition();
    
    picker->Pick(mousePosition[0], mousePosition[1], 0, this->GetDefaultRenderer());
    
    double* worldPosition = picker->GetPickPosition();

    for (QWidget *topLevelWidget : QApplication::topLevelWidgets()) {
    	MainWindow *mainWindow = qobject_cast<MainWindow*>(topLevelWidget);
    	
    	if (mainWindow) {
    		mainWindow->setCoordinate(worldPosition);
    		break;
    	}
    }
    
    vtkInteractorStyleRubberBandPick::OnMouseMove();
}

PickerMode MeshMouseInteractor::getPickerMode() const {
    return pickerMode;
}

void MeshMouseInteractor::activatePicker(const PickerMode &pickerMode) {
    this->pickerMode = pickerMode;
    this->CurrentMode = 0; // VTKISRBP_ORIENT: resets mouse navigation mode
}

void MeshMouseInteractor::deactivatePicker() {
    this->pickerMode = PickerMode::NO_PICKER;
    this->CurrentMode = 0;
}