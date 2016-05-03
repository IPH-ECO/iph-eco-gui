#include <ui/grid_data_vtk_widget.h>

#include <domain/color_gradient.h>
#include <utility/grid_data_mouse_interactor.h>
#include <ui/grid_data_context_menu.h>

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
#include <vtkScalarBarActor.h>
#include <vtkLabeledDataMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkScalarBarRepresentation.h>
#include <vtkInteractorStyleRubberBandZoom.h>

vtkStandardNewMacro(GridDataMouseInteractor);

GridDataVTKWidget::GridDataVTKWidget(QWidget *parent) : MeshVTKWidget(parent, vtkSmartPointer<GridDataMouseInteractor>::New()),
    selectedCellIds(nullptr), currentGridData(nullptr), showMap(true), isCellPickActivated(false)
{
    QObject::connect(this, SIGNAL(mouseEvent(QMouseEvent*)), this, SLOT(handleMouseEvent(QMouseEvent*)));
}

void GridDataVTKWidget::render(Mesh *mesh) {
    clear();
    
    if (!mesh || currentMesh == mesh) {
        return;
    }
    
    currentMesh = mesh;
    vtkPolyData *meshPolyData = currentMesh->getMeshPolyData();
    
    renderer->RemoveActor(meshActor);
    renderer->RemoveActor(axesActor);
    
    GridDataMouseInteractor::SafeDownCast(mouseInteractor)->setMeshPolyData(meshPolyData);
    GridDataMouseInteractor::SafeDownCast(mouseInteractor)->deactivatePicker();
    
    vtkSmartPointer<vtkExtractEdges> extractEdges = vtkSmartPointer<vtkExtractEdges>::New();
    extractEdges->SetInputData(meshPolyData);
    extractEdges->Update();
    
    // Mesh rendering
    vtkSmartPointer<vtkPolyDataMapper> meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    meshMapper->SetInputConnection(extractEdges->GetOutputPort());
    meshMapper->SetResolveCoincidentTopologyToPolygonOffset();
    meshMapper->ScalarVisibilityOff();
    
    meshActor = vtkSmartPointer<vtkActor>::New();
    meshActor->SetMapper(meshMapper);
    meshActor->GetProperty()->LightingOff();
    meshActor->GetProperty()->SetColor(0, 0, 0);
    meshActor->SetVisibility(showMesh);
    
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
    axesActor->SetVisibility(showAxes);
    
    renderer->AddActor(meshActor);
    renderer->AddActor(axesActor);
    renderer->ResetCamera();
}

void GridDataVTKWidget::render(GridData *gridData) {
    Mesh *mesh = gridData->getMesh();
    
    render(mesh);

    currentGridData = gridData;
    layerProperties = gridData->getLayerProperties();
    
    QColor lineColor(layerProperties->getMeshLineColor());
    
    meshActor->GetProperty()->SetColor(lineColor.redF(), lineColor.greenF(), lineColor.blueF());
    meshActor->GetProperty()->SetLineStipplePattern(layerProperties->getMeshLineStyle());
    meshActor->GetProperty()->SetLineWidth(layerProperties->getMeshLineWidth());
    meshActor->GetProperty()->SetOpacity(layerProperties->getMeshOpacity() / 100.0);
    
    vtkSmartPointer<vtkPolyData> inputPointsPolyData = vtkSmartPointer<vtkPolyData>::New();
    inputPointsPolyData->ShallowCopy(currentGridData->getInputPolyData());
    vtkSmartPointer<vtkPoints> inputPoints = inputPointsPolyData->GetPoints();
    
    for (vtkIdType i = 0; i < inputPointsPolyData->GetNumberOfPoints(); i++) {
        double inputPoint[3];
        
        inputPoints->GetPoint(i, inputPoint);
        inputPoints->SetPoint(i, inputPoint[0], inputPoint[1], 0);
    }
    
    vtkColorTransferFunction *pointsColorTransferFunction = buildColorTransferFunction(false);
    vtkSmartPointer<vtkPolyDataMapper> inputPointsMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    
    inputPointsMapper->SetInputData(inputPointsPolyData);
    inputPointsMapper->SetLookupTable(pointsColorTransferFunction);
    inputPointsMapper->UseLookupTableScalarRangeOn();
    inputPointsMapper->SetScalarModeToUsePointData();
    
    mapPointsActor = vtkSmartPointer<vtkActor>::New();
    mapPointsActor->SetMapper(inputPointsMapper);
    mapPointsActor->GetProperty()->SetPointSize(layerProperties->getPointsSize());
    mapPointsActor->GetProperty()->SetOpacity(layerProperties->getPointsOpacity() / 100.0);
    mapPointsActor->SetVisibility(showMapPoints);
    
    QByteArray mapPointsBarActorTitle = currentGridData->getName().toLocal8Bit();
    
    pointsScalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
    pointsScalarBarWidget->SetInteractor(this->GetInteractor());
    pointsScalarBarWidget->GetScalarBarActor()->SetLookupTable(pointsColorTransferFunction);
    pointsScalarBarWidget->GetScalarBarActor()->SetTitle(mapPointsBarActorTitle.constData());
    pointsScalarBarWidget->GetScalarBarActor()->SetNumberOfLabels(4);
    pointsScalarBarWidget->GetScalarBarActor()->SetVisibility(layerProperties->getPointsLegend());
    pointsScalarBarWidget->RepositionableOn();
    pointsScalarBarWidget->SelectableOn();
    pointsScalarBarWidget->ResizableOn();
    pointsScalarBarWidget->EnabledOn();
    
    vtkScalarBarRepresentation *scalarBarRepresentation = vtkScalarBarRepresentation::SafeDownCast(pointsScalarBarWidget->GetRepresentation());
    scalarBarRepresentation->GetPositionCoordinate()->SetValue(0.05, 0.05);
    scalarBarRepresentation->GetPosition2Coordinate()->SetValue(0.1, 0.35);
    
    vtkSmartPointer<vtkPolyData> meshPolyData = mesh->getMeshPolyData();
    QByteArray gridDataName = currentGridData->getName().toLocal8Bit();
    vtkColorTransferFunction *mapColorTransferFunction = buildColorTransferFunction(true);

    meshPolyData->GetCellData()->SetActiveScalars(gridDataName.constData());
    
    vtkSmartPointer<vtkPolyDataMapper> mapMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapMapper->SetInputData(meshPolyData);
    mapMapper->SetLookupTable(mapColorTransferFunction);
    mapMapper->UseLookupTableScalarRangeOn();
    mapMapper->SetScalarModeToUseCellData();
    
    mapActor = vtkSmartPointer<vtkActor>::New();
    mapActor->SetMapper(mapMapper);
    mapActor->GetProperty()->SetOpacity(layerProperties->getMapOpacity() / 100.0);
    mapActor->GetProperty()->SetLighting(layerProperties->getMapLighting());
    mapActor->SetVisibility(showMap);
        
    mapScalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
    mapScalarBarWidget->SetInteractor(this->GetInteractor());
    mapScalarBarWidget->GetScalarBarActor()->SetLookupTable(mapColorTransferFunction);
    mapScalarBarWidget->GetScalarBarActor()->SetTitle("Color Map");
    mapScalarBarWidget->GetScalarBarActor()->SetNumberOfLabels(4);
    mapScalarBarWidget->GetScalarBarActor()->SetVisibility(layerProperties->getMapLegend());
    mapScalarBarWidget->RepositionableOn();
    mapScalarBarWidget->SelectableOn();
    mapScalarBarWidget->ResizableOn();
    mapScalarBarWidget->EnabledOn();
    
    scalarBarRepresentation = vtkScalarBarRepresentation::SafeDownCast(mapScalarBarWidget->GetRepresentation());
    scalarBarRepresentation->GetPositionCoordinate()->SetValue(0.86, 0.05);
    scalarBarRepresentation->GetPosition2Coordinate()->SetValue(0.1, 0.35);
    
    renderer->AddActor(mapActor);
    renderer->AddActor(mapPointsActor);
    
    this->GetRenderWindow()->Render();
}

void GridDataVTKWidget::render() {
    this->render(currentGridData);
}

vtkColorTransferFunction* GridDataVTKWidget::buildColorTransferFunction(bool isColorMap) {
    vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
    QList<QColor> colors;
    bool invertScalarBar;
    double minimumRange;
    double maximumRange;
    
    if (isColorMap) {
        QByteArray gridDataNameByteArray = currentGridData->getName().toLocal8Bit();
        const char *gridDataName = gridDataNameByteArray.data();
        double *range = currentMesh->getMeshPolyData()->GetCellData()->GetScalars(gridDataName)->GetRange();
        
        colors = ColorGradientTemplate::getColors(layerProperties->getMapColorGradient());
        invertScalarBar = layerProperties->getMapInvertColorGradient();
        minimumRange = layerProperties->getUseCustomMapMinimum() ? layerProperties->getCustomMapMininumRange() : range[0];
        maximumRange = layerProperties->getUseCustomMapMaximum() ? layerProperties->getCustomMapMaximumRange() : range[1];
    } else {
        double *range = currentGridData->getInputPolyData()->GetPointData()->GetScalars()->GetRange();
        colors = ColorGradientTemplate::getColors(layerProperties->getPointsColorGradient());
        invertScalarBar = layerProperties->getPointsInvertColorGradient();
        minimumRange = layerProperties->getUseCustomPointsMinimum() ? layerProperties->getCustomPointsMininumRange() : range[0];
        maximumRange = layerProperties->getUseCustomVectorsMaximum() ? layerProperties->getCustomPointsMaximumRange() : range[1];
    }
    
    double interval = maximumRange - minimumRange;

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

void GridDataVTKWidget::toggleMesh(bool show) {
    vtkActor *selectionActor = GridDataMouseInteractor::SafeDownCast(mouseInteractor)->getSelectionActor();
    
    this->showMesh = show;
    this->meshActor->SetVisibility(show);

    if (selectionActor) {
        selectionActor->SetVisibility(show);
        GridDataMouseInteractor::SafeDownCast(mouseInteractor)->getSelectionIdLabelsActor()->SetVisibility(show);
    }
    this->update();
}

void GridDataVTKWidget::toggleMapPoints(bool show) {
    this->showMapPoints = show;
    
    if (layerProperties == nullptr || mapPointsActor == nullptr || pointsScalarBarWidget == nullptr) {
        return;
    }
    
    mapPointsActor->SetVisibility(show);
    pointsScalarBarWidget->GetScalarBarActor()->SetVisibility(show && layerProperties->getPointsLegend());
    this->update();
}

void GridDataVTKWidget::toggleMap(bool show) {
    this->showMap = show;
    
    if (layerProperties == nullptr || mapActor == nullptr || mapScalarBarWidget == nullptr) {
        return;
    }
    
    mapActor->SetVisibility(show);
    mapScalarBarWidget->GetScalarBarActor()->SetVisibility(show && layerProperties->getMapLegend());
    this->update();
}

void GridDataVTKWidget::clear() {
    currentGridData = nullptr;
    layerProperties = nullptr;
    renderer->RemoveActor(mapPointsActor);
    renderer->RemoveActor(mapActor);
    MeshVTKWidget::clear();
}

void GridDataVTKWidget::handleMouseEvent(QMouseEvent *event) {
    if (event->type() == QEvent::MouseButtonDblClick && event->button() == Qt::LeftButton) {
        if (isCellPickActivated) {
            GridDataMouseInteractor::SafeDownCast(mouseInteractor)->pickCell();
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

void GridDataVTKWidget::toggleCellPicker(bool activate, const PickerMode &pickerMode) {
    isCellPickActivated = activate;
    mouseInteractor->deactivatePicker();
    
    if (activate && pickerMode != PickerMode::NO_PICKER) {
        selectedCellIds = vtkSmartPointer<vtkIdTypeArray>::New();
        selectedCellIds->SetName("cellIds");
        selectedCellIds->SetNumberOfComponents(1);
        
        if (pickerMode == PickerMode::MULTIPLE_CELL) {
            mouseInteractor->StartSelect();
        }
        
        GridDataMouseInteractor::SafeDownCast(mouseInteractor)->activatePickerWithCellIds(pickerMode, selectedCellIds);
    }
}

void GridDataVTKWidget::toggleCellLabels(const LabelType &labelType) {
    renderer->RemoveActor2D(labelsActor);
    
    if (labelType != LabelType::UNDEFINED) {
        vtkSmartPointer<vtkCellCenters> cellCentersFilter = vtkSmartPointer<vtkCellCenters>::New();
        cellCentersFilter->SetInputData(currentMesh->getMeshPolyData());
        cellCentersFilter->Update();
        
        vtkSmartPointer<vtkLabeledDataMapper> labelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
        labelMapper->SetInputConnection(cellCentersFilter->GetOutputPort());
        
        if (labelType == LabelType::CELL_ID) {
            labelMapper->SetLabelModeToLabelIds();
        } else {
            labelMapper->SetLabelModeToLabelScalars();
            labelMapper->SetLabelFormat("%.4f");
        }
        
        labelMapper->GetLabelTextProperty()->SetColor(0, 0, 0);
        labelMapper->GetLabelTextProperty()->BoldOff();
        labelMapper->GetLabelTextProperty()->ShadowOff();
        
        labelsActor = vtkSmartPointer<vtkActor2D>::New();
        labelsActor->SetMapper(labelMapper);
        
        renderer->AddActor2D(labelsActor);
    }
    
    this->GetRenderWindow()->Render();
}

void GridDataVTKWidget::lockView(bool lock) {
    if (lock) {
        this->GetInteractor()->Disable();
    } else {
        this->GetInteractor()->Enable();
    }
}