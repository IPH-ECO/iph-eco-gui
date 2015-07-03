#include "include/ui/grid_data_vtk_widget.h"
#include "include/ui/grid_data_context_menu.h"
#include "include/ui/grid_data_dialog.h"

#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkPointData.h>
#include <vtkAreaPicker.h>
#include <vtkDoubleArray.h>
#include <vtkLookupTable.h>
#include <vtkCellCenters.h>
#include <vtkTextProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkLabeledDataMapper.h>

vtkStandardNewMacro(GridDataMouseInteractor);

GridDataVTKWidget::GridDataVTKWidget(QWidget *parent) : QVTKWidget(parent), selectedCellIds(nullptr), currentMesh(nullptr), currentGridData(nullptr),
    showMesh(true), showAxes(true), showColorMap(true), isCellPickActivated(false)
{
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    mouseInteractor = vtkSmartPointer<GridDataMouseInteractor>::New();
    vtkSmartPointer<vtkAreaPicker> areaPicker = vtkSmartPointer<vtkAreaPicker>::New();
    
    renderer->SetBackground(1, 1, 1);
    renderWindow->AddRenderer(renderer);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(mouseInteractor);
    renderWindowInteractor->SetPicker(areaPicker);
    mouseInteractor->SetDefaultRenderer(renderer);
    
    this->SetRenderWindow(renderWindow);
    renderWindow->Render();
    
    GridDataDialog *gridDataDialog = static_cast<GridDataDialog*>(parent);
    QObject::connect(this, SIGNAL(mouseEvent(QMouseEvent*)), this, SLOT(handleMouseEvent(QMouseEvent*)));
    QObject::connect(mouseInteractor, SIGNAL(coordinateChanged(double&, double&)), gridDataDialog, SLOT(setCoordinate(double&, double&)));
}

void GridDataVTKWidget::render(Mesh *mesh) {
    clear();
    
    if (mesh == nullptr || currentMesh == mesh) {
        return;
    }
    
    currentMesh = mesh;
    renderer->RemoveActor(meshActor);
    renderer->RemoveActor(axesActor);
    
    mouseInteractor->setMeshPolyData(currentMesh->getPolyData());
    mouseInteractor->deactivateCellPicking();
    
    // Mesh rendering
    meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    meshMapper->SetInputData(currentMesh->getPolyData());
    meshMapper->ScalarVisibilityOff();
    
    meshActor = vtkSmartPointer<vtkActor>::New();
    meshActor->GetProperty()->LightingOff();
    meshActor->SetMapper(meshMapper);
    
    if (this->showMesh) {
        meshActor->GetProperty()->EdgeVisibilityOn();
    } else {
        meshActor->GetProperty()->EdgeVisibilityOff();
    }
    
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
}

void GridDataVTKWidget::render(GridData *gridData) {
    Mesh *mesh = gridData->getMesh();
    
    render(mesh);

    currentGridData = gridData;
    vtkPolyData *inputPointsPolyData = currentGridData->getInputPolyData();
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
    inputPointsBar->SetTitle(currentGridData->getName().toStdString().c_str());
    inputPointsBar->SetNumberOfLabels(4);
    inputPointsBar->GetTitleTextProperty()->SetFontSize(8); // doesn't have effect
    inputPointsBar->SetWidth(0.1);
    inputPointsBar->SetHeight(0.5);
    inputPointsBar->SetPosition(0.05, 0.05);

    vtkPolyData *meshPolyData = mesh->getPolyData();
    vtkSmartPointer<vtkLookupTable> colorMapTable = vtkSmartPointer<vtkLookupTable>::New();
	std::string gridDataName(currentGridData->getName().toStdString());
    
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
    renderer->AddActor2D(colorMapBar);
    renderer->AddActor2D(inputPointsBar);
}

void GridDataVTKWidget::setShowMesh(bool show) {
    vtkActor *selectionActor = mouseInteractor->getSelectionActor();
    
    this->showMesh = show;
    
    if (show) {
        meshActor->GetProperty()->EdgeVisibilityOn();
        vtkActor *selectionActor = mouseInteractor->getSelectionActor();
        if (selectionActor != nullptr) {
            selectionActor->VisibilityOn();
            mouseInteractor->getSelectionIdLabelsActor()->VisibilityOn();
        }
    } else {
        meshActor->GetProperty()->EdgeVisibilityOff();
        if (selectionActor != nullptr) {
            selectionActor->VisibilityOff();
            mouseInteractor->getSelectionIdLabelsActor()->VisibilityOff();
        }
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
    
    if (currentGridData == nullptr || gridDataActor == nullptr || inputPointsBar == nullptr) {
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
    
    if (currentGridData == nullptr || meshMapper == nullptr || colorMapBar == nullptr) {
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
    currentGridData = nullptr;
    renderer->RemoveActor(gridDataActor);
    renderer->RemoveActor2D(colorMapBar);
    renderer->RemoveActor2D(inputPointsBar);
    this->update();
}

void GridDataVTKWidget::handleMouseEvent(QMouseEvent *event) {
    if (event->type() == QEvent::MouseButtonDblClick && event->button() == Qt::LeftButton) {
        if (isCellPickActivated) {
            mouseInteractor->pickCell();
        }
    } else if (event->type() == QEvent::MouseButtonRelease && event->button() == Qt::RightButton) {
        GridDataContextMenu *contextMenu = new GridDataContextMenu(this);
        QPoint globalPosition = this->mapToGlobal(event->pos());
        bool canEditCellWeights = isCellPickActivated && selectedCellIds->GetNumberOfTuples() > 0;
        
        contextMenu->toggleEditWeightsAction(canEditCellWeights);
        contextMenu->exec(globalPosition);
    }
}

void GridDataVTKWidget::toggleCellPick(bool activate, const CellPickMode &cellPickMode) {
    isCellPickActivated = activate;
    mouseInteractor->deactivateCellPicking();
    
    if (activate && cellPickMode != CellPickMode::UNDEFINED) {
        selectedCellIds = vtkSmartPointer<vtkIdTypeArray>::New();
        selectedCellIds->SetName("cellIds");
        selectedCellIds->SetNumberOfComponents(1);
        
        if (cellPickMode == CellPickMode::MULTIPLE) {
            mouseInteractor->StartSelect();
        }
        mouseInteractor->activateCellPicking(cellPickMode, selectedCellIds);
    }
}

void GridDataVTKWidget::toggleCellLabels(const CellLabelType &cellLabelType) {
    renderer->RemoveActor2D(cellLabelsActor);
    
    if (cellLabelType != CellLabelType::UNDEFINED) {
        vtkSmartPointer<vtkCellCenters> cellCentersFilter = vtkSmartPointer<vtkCellCenters>::New();
        cellCentersFilter->SetInputData(currentMesh->getPolyData());
        cellCentersFilter->Update();
        
        vtkSmartPointer<vtkLabeledDataMapper> labelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
        labelMapper->SetInputConnection(cellCentersFilter->GetOutputPort());
        
        if (cellLabelType == CellLabelType::ID) {
            labelMapper->SetLabelModeToLabelIds();
        } else {
            labelMapper->SetLabelModeToLabelScalars();
        }
        
        labelMapper->GetLabelTextProperty()->SetColor(0, 0, 0);
        labelMapper->GetLabelTextProperty()->ShadowOff();
        
        cellLabelsActor = vtkSmartPointer<vtkActor2D>::New();
        cellLabelsActor->SetMapper(labelMapper);
        
        renderer->AddActor2D(cellLabelsActor);
    }
    
    renderWindow->Render();
}
