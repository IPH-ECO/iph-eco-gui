#include "include/ui/grid_data_vtk_widget.h"

#include "include/domain/mesh.h"
#include "include/ui/grid_data_dialog.h"
#include "include/utility/mouse_interactor.h"

#include <vtkRenderWindowInteractor.h>
#include <vtkWorldPointPicker.h>
#include <vtkTextProperty.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkTextProperty.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkCellData.h>
#include <QList>

GridDataVTKWidget::GridDataVTKWidget(QWidget *parent) : QVTKWidget(parent), showMesh(true), showAxes(true), showGridDataPoints(true) {
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
    meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    meshMapper->SetInputData(mesh->getPolyData());
    meshMapper->ScalarVisibilityOff();
    
    meshActor = vtkSmartPointer<vtkActor>::New();
    meshActor->GetProperty()->LightingOff();
    meshActor->SetMapper(meshMapper);
    
    if (this->showMesh) {
        meshActor->GetProperty()->EdgeVisibilityOn();
    } else {
        meshActor->GetProperty()->EdgeVisibilityOff();
    }
    
    renderer->AddActor(meshActor);
    
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
    axesActor->SetBounds(mesh->getPolyData()->GetBounds());
    axesActor->SetCamera(renderer->GetActiveCamera());
    axesActor->SetFlyModeToStaticEdges();
    
    renderer->AddActor(axesActor);
    
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

    vtkPolyData *inputPointsPolyData = gridData->getInputPolyData();
    vtkSmartPointer<vtkPolyDataMapper> inputPointsMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkSmartPointer<vtkLookupTable> inputPointsTable = vtkSmartPointer<vtkLookupTable>::New();
    double *interval = inputPointsPolyData->GetPointData()->GetScalars()->GetRange();
    
    inputPointsTable->SetTableRange(interval[0], interval[1]);
    
    inputPointsMapper->SetInputData(inputPointsPolyData);
    inputPointsMapper->SetLookupTable(inputPointsTable);
    inputPointsMapper->UseLookupTableScalarRangeOn();
    inputPointsMapper->SetScalarModeToUsePointData();
    
    gridDataActor = vtkSmartPointer<vtkActor>::New();
    gridDataActor->SetMapper(inputPointsMapper);
    
    inputPointsBar = vtkSmartPointer<vtkScalarBarActor>::New();
    inputPointsBar->SetLookupTable(inputPointsTable);
    inputPointsBar->SetTitle(gridData->getName().toStdString().c_str());
    inputPointsBar->SetNumberOfLabels(4);
    inputPointsBar->GetTitleTextProperty()->SetFontSize(8); // doesn't have effect
    inputPointsBar->SetWidth(0.1);
    inputPointsBar->SetHeight(0.5);
    inputPointsBar->SetPosition(0.05, 0.05);

    vtkPolyData *meshPolyData = mesh->getPolyData();
    
    vtkSmartPointer<vtkLookupTable> colorMapTable = vtkSmartPointer<vtkLookupTable>::New();
	std::string gridDataName(gridData->getName().toStdString());
	meshPolyData->GetCellData()->SetActiveScalars(gridDataName.c_str());
	interval = meshPolyData->GetCellData()->GetScalars(gridDataName.c_str())->GetRange();
    colorMapTable->SetTableRange(interval[0], interval[1]);
    
    meshMapper->SetLookupTable(colorMapTable);
    meshMapper->UseLookupTableScalarRangeOn();
    meshMapper->SetScalarModeToUseCellData();
    
    colorMapBar = vtkSmartPointer<vtkScalarBarActor>::New();
    colorMapBar->SetLookupTable(colorMapTable);
    colorMapBar->SetTitle("Color Map");
    colorMapBar->SetNumberOfLabels(4);
    colorMapBar->GetTitleTextProperty()->SetFontSize(8); // doesn't have effect
    colorMapBar->SetWidth(0.1);
    colorMapBar->SetHeight(0.5);
    colorMapBar->SetPosition(0.85, 0.05);
    
    renderer->AddActor2D(colorMapBar);
    
    if (showColorMap) {
        colorMapBar->VisibilityOn();
        meshMapper->ScalarVisibilityOn();
    } else {
        colorMapBar->VisibilityOff();
        meshMapper->ScalarVisibilityOff();
    }
    
    if (showGridDataPoints) {
        gridDataActor->VisibilityOn();
        inputPointsBar->VisibilityOn();
    } else {
        gridDataActor->VisibilityOff();
        inputPointsBar->VisibilityOff();
    }
    
    renderer->AddActor(gridDataActor);
    renderer->AddActor2D(inputPointsBar);
}

void GridDataVTKWidget::setShowMesh(bool show) {
    this->showMesh = show;
    
    if (show) {
        meshActor->GetProperty()->EdgeVisibilityOn();
    } else {
        meshActor->GetProperty()->EdgeVisibilityOff();
    }
    this->update();
}

void GridDataVTKWidget::setShowAxes(bool show) {
    this->showAxes = show;
    
    if (show) {
        axesActor->VisibilityOn();
    } else {
        axesActor->VisibilityOff();
    }
    this->update();
}

void GridDataVTKWidget::setShowGridDataPoints(bool show) {
    this->showGridDataPoints = show;
    
    if (gridDataActor == nullptr || inputPointsBar == nullptr) {
        return;
    }
    
    if (show) {
        gridDataActor->VisibilityOn();
        inputPointsBar->VisibilityOn();
    } else {
        gridDataActor->VisibilityOff();
        inputPointsBar->VisibilityOff();
    }
    this->update();
}

void GridDataVTKWidget::setShowColorMap(bool show) {
    this->showColorMap = show;
    
    if (meshMapper == nullptr || colorMapBar == nullptr) {
        return;
    }
    
    if (show) {
        colorMapBar->VisibilityOn();
        meshMapper->ScalarVisibilityOn();
    } else {
        colorMapBar->VisibilityOff();
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