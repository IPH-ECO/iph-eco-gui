#include "include/ui/grid_data_vtk_widget.h"
#include "include/domain/color_gradient.h"
#include "include/ui/grid_data_context_menu.h"
#include "include/ui/grid_data_dialog.h"

#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkPointData.h>
#include <vtkPNGWriter.h>
#include <vtkAreaPicker.h>
#include <vtkDoubleArray.h>
#include <vtkLookupTable.h>
#include <vtkCellCenters.h>
#include <vtkExtractEdges.h>
#include <vtkTextProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkLabeledDataMapper.h>
#include <vtkWindowToImageFilter.h>
#include <vtkColorTransferFunction.h>
#include <vtkInteractorStyleRubberBandZoom.h>

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
    vtkPolyData *meshPolyData = currentMesh->getPolyData();
    
    renderer->RemoveActor(meshActor);
    renderer->RemoveActor(axesActor);
    
    mouseInteractor->setMeshPolyData(meshPolyData);
    mouseInteractor->deactivateCellPicking();
    
    vtkSmartPointer<vtkExtractEdges> extractEdges = vtkSmartPointer<vtkExtractEdges>::New();
    extractEdges->SetInputData(meshPolyData);
    extractEdges->Update();
    
    // Mesh rendering
    meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    meshMapper->SetInputConnection(extractEdges->GetOutputPort());
    meshMapper->SetResolveCoincidentTopologyToPolygonOffset();
    meshMapper->ScalarVisibilityOff();
    
    meshActor = vtkSmartPointer<vtkActor>::New();
    meshActor->SetMapper(meshMapper);
    meshActor->GetProperty()->LightingOff();
    meshActor->GetProperty()->SetColor(0, 0, 0);
    
    if (this->showMesh) {
        meshActor->VisibilityOn();
    } else {
        meshActor->VisibilityOff();
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
    
    QColor lineColor(gridData->getMeshLineColor());
    
    meshActor->GetProperty()->SetColor(lineColor.redF(), lineColor.greenF(), lineColor.blueF());
    meshActor->GetProperty()->SetLineStipplePattern(gridData->getMeshLineStyle());
    meshActor->GetProperty()->SetLineWidth(gridData->getMeshLineWidth());
    meshActor->GetProperty()->SetOpacity(gridData->getMeshOpacity() / 100.0);
    
    vtkPolyData *inputPointsPolyData = currentGridData->getInputPolyData();
    vtkColorTransferFunction *pointsColorTransferFunction = buildColorTransferFunction(false);
    vtkSmartPointer<vtkPolyDataMapper> inputPointsMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    
    inputPointsMapper->SetInputData(inputPointsPolyData);
    inputPointsMapper->SetLookupTable(pointsColorTransferFunction);
    inputPointsMapper->UseLookupTableScalarRangeOn();
    inputPointsMapper->SetScalarModeToUsePointData();
    
    gridDataActor = vtkSmartPointer<vtkActor>::New();
    gridDataActor->SetMapper(inputPointsMapper);
    gridDataActor->GetProperty()->SetPointSize(gridData->getPointsSize());
    gridDataActor->GetProperty()->SetOpacity(gridData->getPointsOpacity() / 100.0);
    
    std::string inputPointsBarTitle = currentGridData->getName().toStdString();
    
    inputPointsBar = vtkSmartPointer<vtkScalarBarActor>::New();
    inputPointsBar->SetLookupTable(pointsColorTransferFunction);
    inputPointsBar->SetTitle(inputPointsBarTitle.c_str());
    inputPointsBar->SetNumberOfLabels(4);
    inputPointsBar->SetWidth(0.1);
    inputPointsBar->SetHeight(0.5);
    inputPointsBar->SetPosition(0.05, 0.05);

    vtkPolyData *meshPolyData = mesh->getPolyData();
	std::string gridDataName(currentGridData->getName().toStdString());
    vtkColorTransferFunction *mapColorTransferFunction = buildColorTransferFunction(true);

    meshPolyData->GetCellData()->SetActiveScalars(gridDataName.c_str());
    gridMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    gridMapper->SetInputData(meshPolyData);
    gridMapper->SetLookupTable(mapColorTransferFunction);
    gridMapper->UseLookupTableScalarRangeOn();
    gridMapper->SetScalarModeToUseCellData();
    
    gridActor = vtkSmartPointer<vtkActor>::New();
    gridActor->SetMapper(gridMapper);
    gridActor->GetProperty()->SetOpacity(gridData->getMapOpacity() / 100.0);
    
    colorMapBar = vtkSmartPointer<vtkScalarBarActor>::New();
    colorMapBar->SetLookupTable(mapColorTransferFunction);
    colorMapBar->SetTitle("Color Map");
    colorMapBar->SetNumberOfLabels(4);
    colorMapBar->SetWidth(0.1);
    colorMapBar->SetHeight(0.5);
    colorMapBar->SetPosition(0.85, 0.05);
    
    if (gridData->getMapLegend()) {
        colorMapBar->VisibilityOn();
    } else {
        colorMapBar->VisibilityOff();
    }
    
    if (gridData->getPointsLegend()) {
        inputPointsBar->VisibilityOn();
    } else {
        inputPointsBar->VisibilityOff();
    }
    
    if (currentGridData->getMapLighting()) {
        gridActor->GetProperty()->LightingOn();
    } else {
        gridActor->GetProperty()->LightingOff();
    }
    
    if (showColorMap) {
        gridActor->VisibilityOn();
    } else {
        gridActor->VisibilityOff();
    }
    
    if (showGridDataPoints) {
        gridDataActor->VisibilityOn();
    } else {
        gridDataActor->VisibilityOff();
    }
    
    renderer->AddActor(gridActor);
    renderer->AddActor(gridDataActor);
    renderer->AddActor2D(colorMapBar);
    renderer->AddActor2D(inputPointsBar);
}

vtkColorTransferFunction* GridDataVTKWidget::buildColorTransferFunction(bool isColorMap) {
    vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
    QList<QColor> colors;
    bool invertScalarBar;
    double minimumRange;
    double maximumRange;
    double interval;
    
    if (isColorMap) {
        colors = ColorGradientTemplate::getColors(currentGridData->getMapColorGradient());
        invertScalarBar = currentGridData->getMapInvertColorGradient();
        minimumRange = currentGridData->getMapMininumRange();
        maximumRange = currentGridData->getMapMaximumRange();
        interval = currentGridData->getMapMaximumRange() - currentGridData->getMapMininumRange();
    } else {
        colors = ColorGradientTemplate::getColors(currentGridData->getPointsColorGradient());
        invertScalarBar = currentGridData->getPointsInvertColorGradient();
        minimumRange = currentGridData->getPointsMininumRange();
        maximumRange = currentGridData->getPointsMaximumRange();
        interval = currentGridData->getPointsMaximumRange() - currentGridData->getPointsMininumRange();
    }
    
    if (invertScalarBar) {
        for (int i = colors.size() - 1, j = 0; i > 0; i--, j++) {
            double x = minimumRange + j * interval / (double) colors.size();
            colorTransferFunction->AddRGBPoint(x, colors[i].redF(), colors[i].greenF(), colors[i].blueF());
        }
        colorTransferFunction->AddRGBPoint(maximumRange, colors.first().redF(), colors.first().greenF(), colors.first().blueF());
    } else {
        for (int i = 0; i < colors.size() - 1; i++) {
            double x = minimumRange + i * interval / (double) colors.size();
            colorTransferFunction->AddRGBPoint(x, colors[i].redF(), colors[i].greenF(), colors[i].blueF());
        }
        colorTransferFunction->AddRGBPoint(maximumRange, colors.last().redF(), colors.last().greenF(), colors.last().blueF());
    }
    
    return colorTransferFunction;
}

void GridDataVTKWidget::setShowMesh(bool show) {
    vtkActor *selectionActor = mouseInteractor->getSelectionActor();
    
    this->showMesh = show;
    
    if (show) {
        meshActor->VisibilityOn();
        vtkActor *selectionActor = mouseInteractor->getSelectionActor();
        if (selectionActor != nullptr) {
            selectionActor->VisibilityOn();
            mouseInteractor->getSelectionIdLabelsActor()->VisibilityOn();
        }
    } else {
        meshActor->VisibilityOff();
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
        if (currentGridData->getPointsLegend()) {
            inputPointsBar->VisibilityOn();
        } else {
            inputPointsBar->VisibilityOff();
        }
    } else {
        gridDataActor->VisibilityOff();
        inputPointsBar->VisibilityOff();
    }
    this->update();
}

void GridDataVTKWidget::setShowColorMap(bool show) {
    this->showColorMap = show;
    
    if (currentGridData == nullptr || gridActor == nullptr || colorMapBar == nullptr) {
        return;
    }
    
    if (show) {
        gridActor->VisibilityOn();
        if (currentGridData->getMapLegend()) {
            colorMapBar->VisibilityOn();
        } else {
            colorMapBar->VisibilityOff();
        }
    } else {
        gridActor->VisibilityOff();
        colorMapBar->VisibilityOff();
    }
    this->update();
}

void GridDataVTKWidget::changeBackgroundColor(const double &r, const double &g, const double &b) {
    renderer->SetBackground(r, g, b);
    this->update();
}

void GridDataVTKWidget::clear() {
    currentGridData = nullptr;
    renderer->RemoveActor(gridActor);
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
        GridDataContextMenu *contextMenu = new GridDataContextMenu(this->parentWidget()); // GridDataDialog
        QPoint globalPosition = this->mapToGlobal(event->pos());
        bool canEditCellWeights = isCellPickActivated && selectedCellIds->GetNumberOfTuples() > 0;
        
        contextMenu->toggleGridLayerAttributesAction(currentGridData != nullptr);
        contextMenu->toggleEditWeightsAction(canEditCellWeights);
        contextMenu->exec(globalPosition);
    }
}

void GridDataVTKWidget::toggleZoomArea(bool activate) {
    if (activate) {
        vtkSmartPointer<vtkInteractorStyleRubberBandZoom> zoomAreaInteractor = vtkSmartPointer<vtkInteractorStyleRubberBandZoom>::New();
        renderWindowInteractor->SetInteractorStyle(zoomAreaInteractor);
    } else {
        renderWindowInteractor->SetInteractorStyle(mouseInteractor);
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
            labelMapper->SetLabelFormat("%.4f");
        }
        
        labelMapper->GetLabelTextProperty()->SetColor(0, 0, 0);
        labelMapper->GetLabelTextProperty()->BoldOff();
        labelMapper->GetLabelTextProperty()->ShadowOff();
        
        cellLabelsActor = vtkSmartPointer<vtkActor2D>::New();
        cellLabelsActor->SetMapper(labelMapper);
        
        renderer->AddActor2D(cellLabelsActor);
    }
    
    renderWindow->Render();
}

void GridDataVTKWidget::resetZoom() {
    renderer->ResetCamera();
    this->update();
}

void GridDataVTKWidget::lockView(bool lock) {
    if (lock) {
        renderWindowInteractor->Disable();
    } else {
        renderWindowInteractor->Enable();
    }
}

void GridDataVTKWidget::exportToImage(const QString &fileName) {
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(renderWindow);
    windowToImageFilter->SetMagnification(1); //set the resolution of the output image (3 times the current resolution of vtk render window)
    windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
    windowToImageFilter->ReadFrontBufferOff(); // read from the back buffer
    windowToImageFilter->Update();
    
    std::string stdFileName = fileName.toStdString();
    vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetFileName(stdFileName.c_str());
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    writer->Write();
}