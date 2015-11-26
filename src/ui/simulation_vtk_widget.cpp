#include "include/ui/simulation_vtk_widget.h"

#include "include/ui/main_window.h"
#include "include/exceptions/simulation_exception.h"

#include <QFileInfo>
#include <vtkGlyph3D.h>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkDoubleArray.h>
#include <vtkArrowSource.h>
#include <vtkCellCenters.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkScalarBarActor.h>
#include <vtkArrayCalculator.h>
#include <vtkColorTransferFunction.h>
#include <vtkScalarBarRepresentation.h>
#include <vtkGenericDataObjectReader.h>

SimulationVTKWidget::SimulationVTKWidget(QWidget *parent) :
    MeshVTKWidget(parent), currentSimulation(nullptr), layerProperties(nullptr), MAGNITUDE_ARRAY_NAME("magnitudeArray")
{}

void SimulationVTKWidget::render(Simulation *simulation, const QString &layer, const QString &component, int frame) {
    currentSimulation = simulation;
    
    if (layer.isEmpty()) {
        MeshVTKWidget::render(simulation->getMesh());
        return;
    }
    
    QString layerKey = QString("%1-%2").arg(layer).arg(component);
    
    layerProperties = simulation->getSelectedLayers().value(layerKey);
    currentLayer = layer;
    currentComponent = component;
    currentFrame = frame;

    QFileInfoList outputFiles = simulation->getOutputFiles();
    
    if (frame >= outputFiles.size()) {
        throw SimulationException("Frame out of range.");
    }
    
    std::string filename = outputFiles.at(frame).absoluteFilePath().toStdString();
    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();
    
    vtkSmartPointer<vtkUnstructuredGrid> layerGrid = reader->GetUnstructuredGridOutput();
    vtkSmartPointer<vtkActor> actor = layerActors.value(layerKey);
    
    if (!actor) {
        actor = vtkSmartPointer<vtkActor>::New();
    }
    
    vtkSmartPointer<vtkScalarBarWidget> scalarBarWidget = renderScalarBar(actor);
    
    if (component == "Magnitude") {
        vtkSmartPointer<vtkUnstructuredGrid> magnitudeGrid = convertToMagnitudeGrid(layerGrid);
        vtkSmartPointer<vtkDataSetMapper> mapMapper = vtkSmartPointer<vtkDataSetMapper>::New();
        
        mapMapper->SetLookupTable(scalarBarWidget->GetScalarBarActor()->GetLookupTable());
        mapMapper->SetResolveCoincidentTopologyToPolygonOffset();
        mapMapper->SelectColorArray(MAGNITUDE_ARRAY_NAME);
        mapMapper->UseLookupTableScalarRangeOn();
        mapMapper->SetScalarModeToUseCellData();
        mapMapper->SetInputData(magnitudeGrid);
        
        actor->SetMapper(mapMapper);
    } else if (component == "Vector") {
        vtkSmartPointer<vtkPolyData> vectorsPolyData = renderVectors(layerGrid);
        vtkSmartPointer<vtkPolyDataMapper> mapMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        
        mapMapper->SetScalarModeToUsePointData();
        mapMapper->SetInputData(vectorsPolyData);
        mapMapper->UseLookupTableScalarRangeOn();
        mapMapper->SetResolveCoincidentTopologyToPolygonOffset();
        mapMapper->SetLookupTable(scalarBarWidget->GetScalarBarActor()->GetLookupTable());
        
        actor->SetMapper(mapMapper);
    } else {
        vtkSmartPointer<vtkDataSetMapper> mapMapper = vtkSmartPointer<vtkDataSetMapper>::New();
        std::string layerArrayName = layer.toStdString();
        
        mapMapper->SetLookupTable(scalarBarWidget->GetScalarBarActor()->GetLookupTable());
        mapMapper->SetResolveCoincidentTopologyToPolygonOffset();
        mapMapper->SelectColorArray(layerArrayName.c_str());
        mapMapper->UseLookupTableScalarRangeOn();
        mapMapper->SetScalarModeToUseCellData();
        mapMapper->SetInputData(layerGrid);
        
        actor->SetMapper(mapMapper);
    }
    
    if (component == "Vector") {
        actor->GetProperty()->SetOpacity(layerProperties->getVectorsOpacity());
        
        if (layerProperties->getVectorColorMode() == VectorColorMode::CONSTANT) {
            QColor vectorColor = layerProperties->getVectorsColor();
            actor->GetProperty()->SetColor(vectorColor.redF(), vectorColor.greenF(), vectorColor.blueF());
        }
    } else {
        actor->GetProperty()->SetOpacity(layerProperties->getMapOpacity() / 100.0);
        actor->GetProperty()->SetLighting(layerProperties->getMapLighting());
    }
    
    renderer->AddActor(actor);
    this->GetRenderWindow()->Render();
    layerActors.insert(layerKey, actor);
}

vtkSmartPointer<vtkUnstructuredGrid> SimulationVTKWidget::convertToMagnitudeGrid(vtkSmartPointer<vtkUnstructuredGrid> layerGrid) {
    vtkSmartPointer<vtkArrayCalculator> magnitudeFunction = vtkSmartPointer<vtkArrayCalculator>::New();
    std::string vectorsArrayName = currentLayer.toStdString();
    
    magnitudeFunction->AddScalarVariable("x", vectorsArrayName.c_str(), 0);
    magnitudeFunction->AddScalarVariable("y", vectorsArrayName.c_str(), 1);
    magnitudeFunction->AddScalarVariable("z", vectorsArrayName.c_str(), 2);
    magnitudeFunction->SetResultArrayName(MAGNITUDE_ARRAY_NAME);
    magnitudeFunction->SetFunction("sqrt(x^2 + y^2 + z^2)");
    magnitudeFunction->SetAttributeModeToUseCellData();
    magnitudeFunction->SetInputData(layerGrid);
    magnitudeFunction->Update();
    
    return vtkSmartPointer<vtkUnstructuredGrid>(magnitudeFunction->GetUnstructuredGridOutput());
}

vtkSmartPointer<vtkScalarBarWidget> SimulationVTKWidget::renderScalarBar(vtkSmartPointer<vtkActor> layerActor) {
    vtkSmartPointer<vtkScalarBarWidget> scalarBarWidget = scalarBarWidgets.value(layerActor);
    
    if (!scalarBarWidget) {
        std::string scalarBarLabel = currentLayer.toStdString();
        scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
        
        scalarBarWidget->SetInteractor(this->GetInteractor());
        scalarBarWidget->GetScalarBarActor()->SetTitle(scalarBarLabel.c_str());
        scalarBarWidget->GetScalarBarActor()->SetNumberOfLabels(4);
        scalarBarWidget->RepositionableOn();
        scalarBarWidget->SelectableOn();
        scalarBarWidget->ResizableOn();
    }
    
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = buildColorTransferFunction();
    scalarBarWidget->GetScalarBarActor()->SetLookupTable(colorTransferFunction);
    scalarBarWidget->SetEnabled(currentComponent == "Vector" ? layerProperties->getVectorsLegend() : layerProperties->getMapLegend());
    
    vtkScalarBarRepresentation *scalarBarRepresentation = vtkScalarBarRepresentation::SafeDownCast(scalarBarWidget->GetRepresentation());
    scalarBarRepresentation->GetPositionCoordinate()->SetValue(0.86, 0.05);
    scalarBarRepresentation->GetPosition2Coordinate()->SetValue(0.1, 0.35);
    
    scalarBarWidgets.insert(layerActor, scalarBarWidget);
    
    return scalarBarWidget;
}

vtkSmartPointer<vtkPolyData> SimulationVTKWidget::renderVectors(vtkSmartPointer<vtkUnstructuredGrid> layerGrid) {
    vtkSmartPointer<vtkCellCenters> cellCentersFilter = vtkSmartPointer<vtkCellCenters>::New();
    cellCentersFilter->SetInputData(layerGrid);
    cellCentersFilter->Update();
    
    std::string vectorsArrayName = currentLayer.toStdString();
    vtkSmartPointer<vtkDoubleArray> vectorsArray = vtkDoubleArray::SafeDownCast(layerGrid->GetCellData()->GetArray(vectorsArrayName.c_str()));
    vtkSmartPointer<vtkPolyData> arrowsPolyData = vtkSmartPointer<vtkPolyData>::New();
    arrowsPolyData->SetPoints(cellCentersFilter->GetOutput()->GetPoints());
    arrowsPolyData->GetPointData()->SetVectors(vectorsArray);
    
    vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
    arrowSource->SetShaftResolution(10);
    arrowSource->SetTipResolution(10);
    arrowSource->SetShaftRadius(layerProperties->getVectorsWidth() * 0.03); // VTK default
    arrowSource->SetTipRadius(layerProperties->getVectorsWidth() * 0.1); // VTK default
    arrowSource->Update();
    
    vtkSmartPointer<vtkUnstructuredGrid> magnitudeGrid = convertToMagnitudeGrid(layerGrid);
    double *magnitudeRange = magnitudeGrid->GetCellData()->GetArray(MAGNITUDE_ARRAY_NAME)->GetRange();
    vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetSourceConnection(arrowSource->GetOutputPort());
    glyph->SetInputData(arrowsPolyData);
    glyph->SetVectorModeToUseVector();
    glyph->SetScaleModeToScaleByVector();
    glyph->SetColorMode(layerProperties->getVectorColorMode() == VectorColorMode::MAGNITUDE ? VTK_COLOR_BY_VECTOR : VTK_COLOR_BY_SCALE);
    glyph->SetScaleFactor(layerProperties->getVectorsScale() * 1000);
    glyph->SetRange(magnitudeRange);
    glyph->ClampingOn();
    glyph->OrientOn();
    glyph->Update();
    
    return vtkSmartPointer<vtkPolyData>(glyph->GetOutput());
}

vtkSmartPointer<vtkColorTransferFunction> SimulationVTKWidget::buildColorTransferFunction() {
    QList<QColor> colors;
    bool invertScalarBar;
    double minimumRange;
    double maximumRange;
    
    if (currentComponent == "Vector") {
        if (layerProperties->getVectorColorMode() == VectorColorMode::CONSTANT) {
            colors << QColor(layerProperties->getVectorsColor()) << QColor(layerProperties->getVectorsColor());
            invertScalarBar = false;
        } else {
            colors = ColorGradientTemplate::getColors(layerProperties->getVectorsColorGradient());
            invertScalarBar = layerProperties->getVectorsInvertColorGradient();
        }
        
        if (layerProperties->getUseDefaultVectorsValues()) {
            minimumRange = layerProperties->getDefaultVectorsMinimum();
            maximumRange = layerProperties->getDefaultVectorsMaximum();
        } else {
            minimumRange = layerProperties->getVectorsMinimumRange();
            maximumRange = layerProperties->getVectorsMaximumRange();
        }
    } else {
        colors = ColorGradientTemplate::getColors(layerProperties->getMapColorGradient());
        invertScalarBar = layerProperties->getMapInvertColorGradient();
        
        if (layerProperties->getUseDefaultMapValues()) {
            minimumRange = layerProperties->getDefaultMapMinimum();
            maximumRange = layerProperties->getDefaultMapMaximum();
        } else {
            minimumRange = layerProperties->getMapMininumRange();
            maximumRange = layerProperties->getMapMaximumRange();
        }
    }
    
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
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

void SimulationVTKWidget::toggleLayerVisibility(const QString &layerKey, bool visible) {
    vtkSmartPointer<vtkActor> actor = layerActors.value(layerKey);
    
    if (actor) {
        vtkSmartPointer<vtkScalarBarWidget> scalarBarWidget = scalarBarWidgets.value(actor);
        
        if (scalarBarWidget) {
            scalarBarWidget->SetEnabled(visible);
        }
        
        actor->SetVisibility(visible);
        this->GetRenderWindow()->Render();
    }
}

void SimulationVTKWidget::updateLayer() {
    this->render(currentSimulation, currentLayer, currentComponent, currentFrame);
}

void SimulationVTKWidget::removeLayer(const QString &layerKey) {
    vtkSmartPointer<vtkActor> actor = layerActors.value(layerKey);
    
    if (actor) {
        vtkSmartPointer<vtkScalarBarWidget> scalarBarWidget = scalarBarWidgets.value(actor);
        
        if (scalarBarWidget) {
            scalarBarWidget->EnabledOff();
        }
        
        renderer->RemoveActor(actor);
        this->GetRenderWindow()->Render();
        
        scalarBarWidgets.remove(actor);
        layerActors.remove(layerKey);
    }
}