#include "include/ui/grid_data_vtk_widget.h"

#include "include/domain/mesh.h"
#include "include/ui/grid_data_dialog.h"
#include "include/utility/mouse_interactor.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkWorldPointPicker.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <QList>
#include <QDebug>

GridDataVTKWidget::GridDataVTKWidget(QWidget *parent) : QVTKWidget(parent), showMesh(true), showGridDataPoints(true) {
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(1, 1, 1);
}

GridDataVTKWidget::~GridDataVTKWidget() {}

void GridDataVTKWidget::render(Mesh *mesh) {
    if (mesh == nullptr) {
        return;
    }
    
    this->clear();
    
    // Mesh rendering
    vtkPolyData *gridPolyData = mesh->getPolyData();
    vtkSmartPointer<vtkPolyDataMapper> gridMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    gridMapper->SetInputData(gridPolyData);
    
    meshActor = vtkSmartPointer<vtkActor>::New();
    meshActor->GetProperty()->LightingOff();
    meshActor->SetMapper(gridMapper);
    
    if (this->showMesh) {
        meshActor->GetProperty()->EdgeVisibilityOn();
        meshActor->VisibilityOn();
    } else {
        meshActor->GetProperty()->EdgeVisibilityOff();
        meshActor->VisibilityOff();
    }
    
    renderer->AddActor(meshActor);
    
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkWorldPointPicker> worldPointPicker = vtkSmartPointer<vtkWorldPointPicker>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<MouseInteractor> mouseInteractor = vtkSmartPointer<MouseInteractor>::New();
    
    mouseInteractor->SetDefaultRenderer(renderer);
    
    GridDataDialog *gridDataDialog = (GridDataDialog*) this->parent();
    QObject::connect(mouseInteractor, SIGNAL(coordinateChanged(double&, double&)), gridDataDialog, SLOT(setCoordinate(double&, double&)));
    gridDataDialog->setArea(mesh->area());
    
    renderWindowInteractor->SetInteractorStyle(mouseInteractor);
    renderWindowInteractor->SetPicker(worldPointPicker);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    
    renderWindow->AddRenderer(renderer);
    renderer->ResetCamera();
    
    this->SetRenderWindow(renderWindow);
    renderWindow->Render();
}

void GridDataVTKWidget::render(GridData *gridData) {
    Mesh *mesh = gridData->getMesh();
    
    render(mesh);

    vtkPolyData *gridDataInputPolyData = gridData->getInputPolyData();
    vtkSmartPointer<vtkPolyDataMapper> gridDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    gridDataMapper->SetInputData(gridDataInputPolyData);
    
    gridDataActor = vtkSmartPointer<vtkActor>::New();
    gridDataActor->GetProperty()->SetColor(0, 0, 0);
    gridDataActor->GetProperty()->SetRepresentationToPoints();
    gridDataActor->GetProperty()->SetPointSize(3);
    gridDataActor->SetMapper(gridDataMapper);
    
    if (showGridDataPoints) {
        gridDataActor->VisibilityOn();
    } else {
        gridDataActor->VisibilityOff();
    }
    
    renderer->AddActor(gridDataActor);
}

void GridDataVTKWidget::setShowMesh(const bool &value) {
    this->showMesh = value;
    if (value) {
        meshActor->GetProperty()->EdgeVisibilityOn();
        this->meshActor->VisibilityOn();
    } else {
        meshActor->GetProperty()->EdgeVisibilityOff();
        this->meshActor->VisibilityOff();
    }
    this->update();
}

void GridDataVTKWidget::setShowGridDataPoints(const bool &value) {
    this->showGridDataPoints = value;
    if (value) {
        this->gridDataActor->VisibilityOn();
    } else {
        this->gridDataActor->VisibilityOff();
    }
    this->update();
}

void GridDataVTKWidget::setShowInterpolationResult(const bool &value) {
    
}

void GridDataVTKWidget::clear() {
    if (renderer != nullptr) {
        renderer->RemoveAllViewProps();
        this->update();
    }
}