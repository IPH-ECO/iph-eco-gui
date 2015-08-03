#include "include/ui/hydrodynamic_vtk_widget.h"

#include "include/ui/hydrodynamic_data_dialog.h"
#include "include/utility/hydrodynamic_mouse_interactor.h"

#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkPolyDataMapper.h>
#include <vtkTextProperty.h>
#include <vtkAreaPicker.h>
#include <vtkProperty.h>

vtkStandardNewMacro(HydrodynamicMouseInteractor);

HydrodynamicVTKWidget::HydrodynamicVTKWidget(QWidget *parent) :
    QVTKWidget(parent), currentMesh(nullptr), showAxes(true), showMesh(true), isCellPickActivated(false)
{
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
    
    selectedCellIds = vtkSmartPointer<vtkIdTypeArray>::New();
    selectedCellIds->SetName("cellIds");
    selectedCellIds->SetNumberOfComponents(1);

    HydrodynamicDataDialog *hydrodynamicDataDialog = static_cast<HydrodynamicDataDialog*>(parent);
    QObject::connect(mouseInteractor, SIGNAL(coordinateChanged(double&, double&)), hydrodynamicDataDialog, SLOT(setCoordinate(double&, double&)));
    QObject::connect(this, SIGNAL(mouseEvent(QMouseEvent*)), this, SLOT(handleMouseEvent(QMouseEvent*)));
}

void HydrodynamicVTKWidget::render(Mesh *mesh) {
    if (mesh == nullptr || currentMesh == mesh) {
        return;
    }
    
    currentMesh = mesh;
    vtkPolyData *meshPolyData = currentMesh->getPolyData();
    
    renderer->RemoveActor(meshActor);
    renderer->RemoveActor(axesActor);
    
    mouseInteractor->setMeshPolyData(meshPolyData);
    
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
    axesActor->SetBounds(currentMesh->getPolyData()->GetBounds());
    axesActor->SetCamera(renderer->GetActiveCamera());
    axesActor->SetFlyModeToStaticEdges();
    
    if (this->showAxes) {
        axesActor->VisibilityOn();
    } else {
        axesActor->VisibilityOff();
    }
    
    renderer->AddActor(meshActor);
    renderer->AddActor(axesActor);
    renderer->ResetCamera();
    this->update();
}

void HydrodynamicVTKWidget::toggleAxes(bool show) {
    this->showAxes = show;
    
    if (axesActor != nullptr) {
        if (show) {
            axesActor->VisibilityOn();
        } else {
            axesActor->VisibilityOff();
        }
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
        if (show) {
            meshActor->VisibilityOn();
        } else {
            meshActor->VisibilityOff();
        }
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

void HydrodynamicVTKWidget::toggleCellPick(bool activate, const CellPickMode &cellPickMode) {
    isCellPickActivated = activate;
    
    if (activate) {
        if (cellPickMode != CellPickMode::UNDEFINED) {
            if (cellPickMode == CellPickMode::MULTIPLE) {
                mouseInteractor->StartSelect();
            }
            mouseInteractor->activateCellPicker(cellPickMode, selectedCellIds);
        }
    } else {
        mouseInteractor->deactivateCellPicker();
    }
}

vtkIdTypeArray* HydrodynamicVTKWidget::getSelectedCellIds() const {
    return selectedCellIds;
}

void HydrodynamicVTKWidget::handleMouseEvent(QMouseEvent *event) {
    if (event->type() == QEvent::MouseButtonDblClick && event->button() == Qt::LeftButton && isCellPickActivated) {
        mouseInteractor->pickCell();
    }
}