#include "include/ui/grid_data_vtk_widget.h"

#include "include/domain/mesh.h"
#include "include/ui/grid_data_dialog.h"
#include "include/utility/mouse_interactor.h"

#include <vtkSmartPointer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkWorldPointPicker.h>
#include <vtkScalarBarActor.h>
#include <vtkRenderWindow.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <QList>

GridDataVTKWidget::GridDataVTKWidget(QWidget *parent) : QVTKWidget(parent), showMesh(true), showGridDataPoints(true), showInterpolationResult(false) {
    gridDataActor = vtkSmartPointer<vtkActor>::New();
    meshActor = vtkSmartPointer<vtkActor>::New();
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(1, 1, 1);
}

void GridDataVTKWidget::render(Mesh *mesh) {
	GridDataDialog *gridDataDialog = (GridDataDialog*) this->parent();

    if (mesh == nullptr) {
		gridDataDialog->setArea(0);
        return;
    }
    
    this->clear();
    
    // Mesh rendering
    vtkPolyData *gridPolyData = mesh->getPolyData();
    meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    
    meshMapper->SetInputData(gridPolyData);
    meshMapper->SetScalarModeToUseCellData();
    meshMapper->SetColorModeToMapScalars();
    if (this->showInterpolationResult) {
        meshMapper->ScalarVisibilityOn();
    } else {
        meshMapper->ScalarVisibilityOff();
    }
    
    meshActor->SetMapper(meshMapper);
    
    if (this->showMesh) {
        meshActor->GetProperty()->EdgeVisibilityOn();
    } else {
        meshActor->GetProperty()->EdgeVisibilityOff();
    }
    
    renderer->AddActor(meshActor);
    
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkWorldPointPicker> worldPointPicker = vtkSmartPointer<vtkWorldPointPicker>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<MouseInteractor> mouseInteractor = vtkSmartPointer<MouseInteractor>::New();
    
    mouseInteractor->SetDefaultRenderer(renderer);
    
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
    double *interval = gridDataInputPolyData->GetPointData()->GetScalars()->GetRange();
    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    
    lut->SetTableRange(interval[0], interval[1]);
    
    gridDataMapper->SetInputData(gridDataInputPolyData);
    gridDataMapper->SetLookupTable(lut);
    gridDataMapper->UseLookupTableScalarRangeOn();
    gridDataMapper->SetScalarModeToUsePointData();
    
    if (showGridDataPoints) {
        gridDataActor->VisibilityOn();
    } else {
        gridDataActor->VisibilityOff();
    }
    
    gridDataActor->SetMapper(gridDataMapper);
    
    vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBar->SetTitle(gridData->getName().toStdString().c_str());
    scalarBar->SetNumberOfLabels(10);
    scalarBar->SetLookupTable(lut);
    
    renderer->AddActor(gridDataActor);
    renderer->AddActor2D(scalarBar);
}

void GridDataVTKWidget::setShowMesh(bool value) {
    this->showMesh = value;
    if (value) {
        meshActor->GetProperty()->EdgeVisibilityOn();
    } else {
        meshActor->GetProperty()->EdgeVisibilityOff();
    }
    this->update();
}

void GridDataVTKWidget::setShowGridDataPoints(bool show) {
    this->showGridDataPoints = show;
    if (show) {
        gridDataActor->VisibilityOn();
    } else {
        gridDataActor->VisibilityOff();
    }
    this->update();
}

void GridDataVTKWidget::setShowInterpolationResult(bool show) {
    this->showInterpolationResult = show;
    if (show) {
        meshMapper->ScalarVisibilityOn();
    } else {
        meshMapper->ScalarVisibilityOff();
    }
    this->update();
}

void GridDataVTKWidget::changeBackgroundColor(const double &r, const double &g, const double &b) {
    meshActor->GetProperty()->SetColor(r, g, b);
    renderer->SetBackground(r, g, b);
    this->update();
}

void GridDataVTKWidget::clear() {
    if (renderer != nullptr) {
        renderer->RemoveAllViewProps();
        this->update();
    }
}