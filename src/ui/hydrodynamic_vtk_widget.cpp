#include <ui/hydrodynamic_vtk_widget.h>

#include <ui/main_window.h>

#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkPolyDataMapper.h>
#include <vtkTextProperty.h>
#include <vtkAreaPicker.h>
#include <vtkProperty.h>
#include <QApplication>

vtkStandardNewMacro(HydrodynamicMouseInteractor);

HydrodynamicVTKWidget::HydrodynamicVTKWidget(QWidget *parent) : MeshVTKWidget(parent, vtkSmartPointer<HydrodynamicMouseInteractor>::New()) {
    QObject::connect(this, SIGNAL(mouseEvent(QMouseEvent*)), this, SLOT(handleMouseEvent(QMouseEvent*)));
}

void HydrodynamicVTKWidget::render(HydrodynamicConfiguration *hydrodynamicConfiguration) {
    Mesh *mesh = hydrodynamicConfiguration->getGridDataConfiguration()->getMesh();
    
    if (mesh == nullptr || currentMesh == mesh) {
        return;
    }
    
    currentMesh = mesh;
    vtkSmartPointer<vtkPolyData> meshPolyData = currentMesh->getMeshPolyData();
    
    renderer->RemoveActor(meshActor);
    renderer->RemoveActor(axesActor);
    
    // Mesh rendering
    vtkSmartPointer<vtkPolyDataMapper> meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    meshMapper->SetInputData(meshPolyData);
    meshMapper->SetResolveCoincidentTopologyToDefault();
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
    
    HydrodynamicMouseInteractor::SafeDownCast(mouseInteractor)->setHydrodynamicConfiguration(hydrodynamicConfiguration);
    
    renderer->ResetCamera();
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
    return HydrodynamicMouseInteractor::SafeDownCast(mouseInteractor);
}

void HydrodynamicVTKWidget::handleMouseEvent(QMouseEvent *event) {
    if (event->type() == QEvent::MouseButtonDblClick && event->button() == Qt::LeftButton && mouseInteractor->getPickerMode() != PickerMode::NO_PICKER) {
        bool wasCellPicked = HydrodynamicMouseInteractor::SafeDownCast(mouseInteractor)->pickCell();
        
        if (wasCellPicked) {
            emit objectSelected();
        }
    }
}