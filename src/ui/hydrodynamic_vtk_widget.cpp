#include "include/ui/hydrodynamic_vtk_widget.h"

#include "include/ui/main_window.h"
#include "include/utility/hydrodynamic_mouse_interactor.h"

#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkPolyDataMapper.h>
#include <vtkTextProperty.h>
#include <vtkAreaPicker.h>
#include <vtkProperty.h>

vtkStandardNewMacro(HydrodynamicMouseInteractor);

HydrodynamicVTKWidget::HydrodynamicVTKWidget(QWidget *parent) : MeshVTKWidget(parent) {
    hydrodynamicMouseInteractor = vtkSmartPointer<HydrodynamicMouseInteractor>::New();
    renderWindowInteractor->SetInteractorStyle(hydrodynamicMouseInteractor);
    hydrodynamicMouseInteractor->SetDefaultRenderer(renderer);
}

void HydrodynamicVTKWidget::render(HydrodynamicConfiguration *hydrodynamicConfiguration) {
    Mesh *mesh = hydrodynamicConfiguration->getGridDataConfiguration()->getMesh();
    
    if (mesh == nullptr || currentMesh == mesh) {
        return;
    }
    
    hydrodynamicMouseInteractor->setHydrodynamicConfiguration(hydrodynamicConfiguration);
    MeshVTKWidget::render(mesh);
}

void HydrodynamicVTKWidget::togglePicker(bool activate, const PickerMode &pickerMode) {
    if (activate && pickerMode != PickerMode::NO_PICKER) {
        hydrodynamicMouseInteractor->activatePicker(pickerMode);
        
        if (pickerMode == PickerMode::MULTIPLE_CELL) {
            mouseInteractor->StartSelect();
        } else if (pickerMode == PickerMode::MULTIPLE_EDGE) {
            mouseInteractor->StartSelect();
            meshActor->PickableOff();
        }
    } else {
        meshActor->PickableOn();
        hydrodynamicMouseInteractor->deactivatePicker();
    }
}

HydrodynamicMouseInteractor* HydrodynamicVTKWidget::getMouseInteractor() const {
    return hydrodynamicMouseInteractor;
}

void HydrodynamicVTKWidget::handleMouseEvent(QMouseEvent *event) {
    if (event->type() == QEvent::MouseButtonDblClick && event->button() == Qt::LeftButton && hydrodynamicMouseInteractor->getPickerMode() != PickerMode::NO_PICKER) {
        hydrodynamicMouseInteractor->pickCell();
    }
}