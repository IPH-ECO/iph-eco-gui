#include "include/ui/grid_data_vtk_widget.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkWorldPointPicker.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <QList>
#include <QDebug>

#include "include/ui/grid_data_dialog.h"
#include "include/domain/mesh.h"
#include "include/utility/mouse_interactor.h"

GridDataVTKWidget::GridDataVTKWidget(QWidget *parent) : QVTKWidget(parent) {}

GridDataVTKWidget::~GridDataVTKWidget() {}

void GridDataVTKWidget::render(GridDataConfiguration *configuration) {
    Mesh *mesh = configuration->getMesh();

    if (mesh == NULL) {
        return;
    }

    MeshPolygon *boundaryPolygon = mesh->getBoundaryPolygon();

    if (boundaryPolygon->getFilteredPolygon() == NULL) {
        return;
    }

    // Mesh rendering
    vtkSmartPointer<vtkPolyData> gridPolyData = mesh->getGrid();
    vtkSmartPointer<vtkPolyDataMapper> gridMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    gridMapper->InterpolateScalarsBeforeMappingOff();
    gridMapper->SetInputData(gridPolyData);
    
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(gridMapper);
    actor->GetProperty()->EdgeVisibilityOff();

    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(1, 1, 1);
    renderer->AddActor(actor);

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

    this->SetRenderWindow(renderWindow);
    renderWindow->Render();
}

void GridDataVTKWidget::clear() {
    if (renderer != NULL) {
        renderer->RemoveAllViewProps();
        this->update();
    }
}