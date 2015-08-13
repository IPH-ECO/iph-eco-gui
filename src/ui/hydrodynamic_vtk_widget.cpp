#include "include/ui/hydrodynamic_vtk_widget.h"

#include "include/ui/hydrodynamic_data_dialog.h"
#include "include/utility/hydrodynamic_mouse_interactor.h"

#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkPolyDataMapper.h>
#include <vtkTextProperty.h>
#include <vtkAreaPicker.h>
#include <vtkProperty.h>

vtkStandardNewMacro(HydrodynamicMouseInteractor);

HydrodynamicVTKWidget::HydrodynamicVTKWidget(QWidget *parent) : QVTKWidget(parent), currentMesh(nullptr), showAxes(true), showMesh(true) {
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    mouseInteractor = vtkSmartPointer<HydrodynamicMouseInteractor>::New();
    vtkSmartPointer<vtkAreaPicker> areaPicker = vtkSmartPointer<vtkAreaPicker>::New();
    
    renderer->SetBackground(1, 1, 1);
    renderWindow->AddRenderer(renderer);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(mouseInteractor);
    renderWindowInteractor->SetPicker(areaPicker);
    mouseInteractor->SetDefaultRenderer(renderer);
    
    this->SetRenderWindow(renderWindow);
    renderWindow->Render();

    HydrodynamicDataDialog *hydrodynamicDataDialog = static_cast<HydrodynamicDataDialog*>(parent);
    QObject::connect(mouseInteractor, SIGNAL(coordinateChanged(double&, double&)), hydrodynamicDataDialog, SLOT(setCoordinate(double&, double&)));
    QObject::connect(this, SIGNAL(mouseEvent(QMouseEvent*)), this, SLOT(handleMouseEvent(QMouseEvent*)));
}

void HydrodynamicVTKWidget::render(HydrodynamicConfiguration *hydrodynamicConfiguration) {
    Mesh *mesh = hydrodynamicConfiguration->getGridDataConfiguration()->getMesh();
    
    if (mesh == nullptr || currentMesh == mesh) {
        return;
    }
    
    currentMesh = mesh;
    vtkPolyData *meshPolyData = currentMesh->getMeshPolyData();
    
    renderer->RemoveActor(meshActor);
    renderer->RemoveActor(axesActor);
    
    // Mesh rendering
    vtkSmartPointer<vtkPolyDataMapper> meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    meshMapper->SetInputData(meshPolyData);
    meshMapper->SetResolveCoincidentTopologyToPolygonOffset();
    meshMapper->ScalarVisibilityOff();
    
    meshActor = vtkSmartPointer<vtkActor>::New();
    meshActor->SetMapper(meshMapper);
    meshActor->GetProperty()->LightingOff();
    meshActor->GetProperty()->SetColor(1, 1, 1);
    meshActor->GetProperty()->EdgeVisibilityOn();
    
    axesActor = vtkSmartPointer<vtkCubeAxesActor>::New();
    axesActor->SetXUnits("m");
    axesActor->SetXLabelFormat("%4.2f");
    axesActor->SetYUnits("m");
    axesActor->SetYLabelFormat("%4.2f");
    axesActor->ZAxisVisibilityOff();
    axesActor->GetLabelTextProperty(0)->SetColor(0, 0, 0);
    axesActor->GetTitleTextProperty(0)->SetColor(0, 0, 0);
    axesActor->GetXAxesLinesProperty()->SetColor(0, 0, 0);
    axesActor->GetLabelTextProperty(1)->SetColor(0, 0, 0);
    axesActor->GetTitleTextProperty(1)->SetColor(0, 0, 0);
    axesActor->GetYAxesLinesProperty()->SetColor(0, 0, 0);
    axesActor->SetBounds(currentMesh->getMeshPolyData()->GetBounds());
    axesActor->SetCamera(renderer->GetActiveCamera());
    axesActor->SetFlyModeToStaticEdges();
    axesActor->SetVisibility(this->showAxes);
    
    renderer->AddActor(meshActor);
    renderer->AddActor(axesActor);
    
    mouseInteractor->setHydrodynamicConfiguration(hydrodynamicConfiguration);
    
    renderer->ResetCamera();
    this->update();
}

void HydrodynamicVTKWidget::toggleAxes(bool show) {
    this->showAxes = show;
    
    if (axesActor != nullptr) {
        axesActor->SetVisibility(show);
        this->update();
    }
}

void HydrodynamicVTKWidget::resetZoom() {
    renderer->ResetCamera();
    this->update();
}

void HydrodynamicVTKWidget::toggleZoomArea(bool activate) {
    if (activate) {
        vtkSmartPointer<vtkInteractorStyleRubberBandZoom> zoomAreaInteractor = vtkSmartPointer<vtkInteractorStyleRubberBandZoom>::New();
        renderWindowInteractor->SetInteractorStyle(zoomAreaInteractor);
    } else {
        renderWindowInteractor->SetInteractorStyle(mouseInteractor);
    }
}

void HydrodynamicVTKWidget::toggleMesh(bool show) {
    this->showMesh = show;
    
    if (axesActor != nullptr) {
        meshActor->SetVisibility(show);
        this->update();
    }
}

void HydrodynamicVTKWidget::changeBackgroundColor(const double &r, const double &g, const double &b) {
    if (meshActor != nullptr) {
        meshActor->GetProperty()->SetColor(r, g, b);
    }
    
    renderer->SetBackground(r, g, b);
    this->update();
}

void HydrodynamicVTKWidget::togglePicker(bool activate, const PickerMode &pickerMode) {
    if (activate && pickerMode != PickerMode::NO_PICKER) {
        mouseInteractor->activatePicker(pickerMode);
        
        if (pickerMode == PickerMode::MULTIPLE_CELL) {
            mouseInteractor->StartSelect();
        } else if (pickerMode == PickerMode::MULTIPLE_EDGE) {
            mouseInteractor->StartSelect();
            meshActor->PickableOff();
        }
    } else {
        meshActor->PickableOn();
        mouseInteractor->deactivatePicker();
    }
}

HydrodynamicMouseInteractor* HydrodynamicVTKWidget::getMouseInteractor() const {
    return mouseInteractor;
}

void HydrodynamicVTKWidget::handleMouseEvent(QMouseEvent *event) {
    if (event->type() == QEvent::MouseButtonDblClick && event->button() == Qt::LeftButton && mouseInteractor->getPickerMode() != PickerMode::NO_PICKER) {
        mouseInteractor->pickCell();
    }
}