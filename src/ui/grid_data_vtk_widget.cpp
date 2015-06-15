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
#include <QDebug>

GridDataVTKWidget::GridDataVTKWidget(QWidget *parent) : QVTKWidget(parent), showMesh(true), showGridDataPoints(true), showInterpolationResult(false) {
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
    meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    
    meshMapper->SetInputData(gridPolyData);
    meshMapper->SetScalarModeToUseCellData();
    if (this->showInterpolationResult) {
        meshMapper->ScalarVisibilityOn();
    } else {
        meshMapper->ScalarVisibilityOff();
    }
    
    meshActor = vtkSmartPointer<vtkActor>::New();
    meshActor->GetProperty()->LightingOff();
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
    double *interval = gridDataInputPolyData->GetPointData()->GetScalars()->GetRange();
    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();

    lut->SetTableRange(interval[0], interval[1]);
//    lut->SetHueRange(0, 1);
//    lut->SetSaturationRange(0, 1);
//    lut->SetValueRange(0, 1);
//    lut->SetAlphaRange(0, 1);
//    lut->Build();
    
    gridDataMapper->SetInputData(gridDataInputPolyData);
    gridDataMapper->UseLookupTableScalarRangeOn();
    gridDataMapper->SetLookupTable(lut);
    gridDataMapper->ScalarVisibilityOn();
    gridDataMapper->SetScalarModeToUsePointData();
    gridDataMapper->SetColorModeToMapScalars();
    
    vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBar->SetTitle(gridData->gridDataTypeToString().toStdString().c_str());
    scalarBar->SetNumberOfLabels(5);
    scalarBar->SetLookupTable(lut);
    
    gridDataActor = vtkSmartPointer<vtkActor>::New();
    gridDataActor->SetMapper(gridDataMapper);
    
    if (showGridDataPoints) {
        gridDataActor->VisibilityOn();
    } else {
        gridDataActor->VisibilityOff();
    }
    
    renderer->AddActor(gridDataActor);
    renderer->AddActor2D(scalarBar);
}

void GridDataVTKWidget::setShowMesh(const bool &value) {
    this->showMesh = value;
    if (value) {
        meshActor->GetProperty()->EdgeVisibilityOn();
    } else {
        meshActor->GetProperty()->EdgeVisibilityOff();
    }
    this->update();
}

void GridDataVTKWidget::setShowGridDataPoints(const bool &value) {
    this->showGridDataPoints = value;
    if (value) {
        gridDataActor->VisibilityOn();
    } else {
        gridDataActor->VisibilityOff();
    }
    this->update();
}

void GridDataVTKWidget::setShowInterpolationResult(const bool &value) {
    this->showInterpolationResult = value;
    if (this->showInterpolationResult) {
        meshMapper->ScalarVisibilityOn();
    } else {
        meshMapper->ScalarVisibilityOff();
    }
    this->update();
}

void GridDataVTKWidget::clear() {
    if (renderer != nullptr) {
        renderer->RemoveAllViewProps();
        this->update();
    }
}