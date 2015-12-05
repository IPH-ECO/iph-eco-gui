#include "include/ui/simulation_vtk_widget.h"

#include "include/ui/main_window.h"
#include "include/exceptions/simulation_exception.h"

#include <vtkGlyph3D.h>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkDoubleArray.h>
#include <vtkArrowSource.h>
#include <vtkCellCenters.h>
#include <vtkPolyDataMapper.h>
#include <vtkScalarBarActor.h>
#include <vtkGeometryFilter.h>
#include <vtkArrayCalculator.h>
#include <vtkColorTransferFunction.h>
#include <vtkScalarBarRepresentation.h>
#include <vtkGenericDataObjectReader.h>

SimulationVTKWidget::SimulationVTKWidget(QWidget *parent) :
    MeshVTKWidget(parent),
    currentSimulation(nullptr),
    layerProperties(nullptr),
    MAGNITUDE_ARRAY_NAME("VectorMagnitude")
{}

void SimulationVTKWidget::render(Simulation *simulation, const QString &layer, const QString &component, int frame) {
    if (currentSimulation != simulation) {
        outputFiles = simulation->getOutputFiles();
        
        if (outputFiles.isEmpty()) {
            throw SimulationException("Output files for this simulation were not found.");
        }
        
        currentSimulation = simulation;
        currentMesh = simulation->getMesh();
        
        layerArrayMap.clear();
        
        renderMeshLayer();
    }
    
    if (layer.isEmpty()) {
        return;
    }
    
    if (frame >= outputFiles.size()) {
        throw SimulationException("Frame out of range.");
    }
    
    QString layerKey = QString("%1-%2").arg(layer).arg(component);
    
    layerProperties = simulation->getSelectedLayers().value(layerKey);
    currentLayer = layer;
    currentComponent = component;
    currentFrame = frame;
    
    readFrame(frame);
    
    vtkSmartPointer<vtkDoubleArray> layerArray = layerArrayMap.value(layerKey);
    std::string layerArrayName = layer.toStdString();
    double layerRange[2];
    
    if (component == "Magnitude") {
        layerArray = this->buildMagnitudeArray();
        layerArray->GetRange(layerRange);
        layerGrid->GetCellData()->AddArray(layerArray);
        meshActor->GetProperty()->SetRepresentationToSurface();
    } else if (component == "Vector") {
        layerArrayName = MAGNITUDE_ARRAY_NAME;
    } else { // Arbritary layer
        layerGrid->GetCellData()->GetArray(layerArrayName.c_str())->GetRange(layerRange);
        meshActor->GetProperty()->SetRepresentationToSurface();
    }
    
    if (component == "Vector") {
        vtkSmartPointer<vtkPolyData> vectorsPolyData = renderVectors();
        vtkSmartPointer<vtkActor> vectorsActor = vtkSmartPointer<vtkActor>::New();
        
        vectorsPolyData->GetPointData()->GetArray(layerArrayName.c_str())->GetRange(layerRange);
        
        vtkSmartPointer<vtkScalarBarWidget> scalarBarWidget = renderScalarBar(layerKey, layerRange);
        
        vtkSmartPointer<vtkPolyDataMapper> vectorsPolyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        vectorsPolyDataMapper->SetInputData(vectorsPolyData);
        vectorsPolyDataMapper->SetScalarModeToUsePointData();
        vectorsPolyDataMapper->UseLookupTableScalarRangeOn();
        vectorsPolyDataMapper->SetLookupTable(scalarBarWidget->GetScalarBarActor()->GetLookupTable());
        
        vectorsActor->SetMapper(vectorsPolyDataMapper);
        
        if (vectorActors.value(layerKey)) {
            renderer->RemoveActor(vectorActors.take(layerKey));
        }
        
        vectorActors.insert(layerKey, vectorsActor);
        renderer->AddActor(vectorsActor);
    } else {
        vtkSmartPointer<vtkScalarBarWidget> scalarBarWidget = renderScalarBar(layerKey, layerRange);
        
        layerDataSetMapper->SetInputData(layerGrid);
        layerDataSetMapper->SetLookupTable(scalarBarWidget->GetScalarBarActor()->GetLookupTable());
        layerDataSetMapper->SelectColorArray(layerArrayName.c_str());
        layerDataSetMapper->UseLookupTableScalarRangeOn();
        layerDataSetMapper->SetScalarModeToUseCellData();
        layerDataSetMapper->ScalarVisibilityOn();
    }
    
//    if (component == "Vector") {
//        layerActor->GetProperty()->SetOpacity(layerProperties->getVectorsOpacity());
//        
//        if (layerProperties->getVectorColorMode() == VectorColorMode::CONSTANT) {
//            QColor vectorColor = layerProperties->getVectorsColor();
//            layerActor->GetProperty()->SetColor(vectorColor.redF(), vectorColor.greenF(), vectorColor.blueF());
//        }
//    } else {
//        layerActor->GetProperty()->SetOpacity(layerProperties->getMapOpacity() / 100.0);
//        layerActor->GetProperty()->SetLighting(layerProperties->getMapLighting());
//    }
    
    this->GetRenderWindow()->Render();
}

void SimulationVTKWidget::readFrame(const int frame) {
    std::string fileName = outputFiles.at(frame).absoluteFilePath().toStdString();
    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(fileName.c_str());
    reader->Update();
    
    layerGrid = reader->GetUnstructuredGridOutput();
}

void SimulationVTKWidget::renderMeshLayer() {
    this->readFrame(0);
    
    layerDataSetMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    layerDataSetMapper->SetInputData(layerGrid);
    layerDataSetMapper->ScalarVisibilityOff();
    
    this->renderer->RemoveActor(meshActor);
    
    QColor meshColor(currentMesh->getColor());
    
    meshActor = vtkSmartPointer<vtkActor>::New();
    meshActor->SetMapper(layerDataSetMapper);
    meshActor->GetProperty()->SetRepresentationToWireframe();
    meshActor->GetProperty()->EdgeVisibilityOn();
    meshActor->GetProperty()->SetColor(meshColor.redF(), meshColor.greenF(), meshColor.blueF());
    meshActor->GetProperty()->LightingOff();
    meshActor->SetScale(1, 1, 100);
    
    this->renderer->AddActor(meshActor);
    this->renderAxesActor();
    this->renderer->ResetCamera();
    this->GetRenderWindow()->Render();
}

vtkSmartPointer<vtkDoubleArray> SimulationVTKWidget::buildMagnitudeArray() {
    vtkSmartPointer<vtkArrayCalculator> magnitudeFunction = vtkSmartPointer<vtkArrayCalculator>::New();
    std::string vectorsArrayName = currentLayer.toStdString();
    
    magnitudeFunction->AddScalarVariable("x", vectorsArrayName.c_str(), 0);
    magnitudeFunction->AddScalarVariable("y", vectorsArrayName.c_str(), 1);
    magnitudeFunction->AddScalarVariable("z", vectorsArrayName.c_str(), 2);
    magnitudeFunction->SetResultArrayName(vectorsArrayName.c_str());
    magnitudeFunction->SetFunction("sqrt(x^2 + y^2 + z^2)");
    magnitudeFunction->SetAttributeModeToUseCellData();
    magnitudeFunction->SetInputData(layerGrid);
    magnitudeFunction->Update();
    
    vtkSmartPointer<vtkUnstructuredGrid> magnitudePolyData = magnitudeFunction->GetUnstructuredGridOutput();
    
    return vtkSmartPointer<vtkDoubleArray>(vtkDoubleArray::SafeDownCast(magnitudePolyData->GetCellData()->GetArray(vectorsArrayName.c_str())));
}

vtkSmartPointer<vtkScalarBarWidget> SimulationVTKWidget::renderScalarBar(const QString &layerKey, double *scalarBarRange) {
    vtkSmartPointer<vtkScalarBarWidget> scalarBarWidget = scalarBarWidgets.value(layerKey);
    
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
    
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = buildColorTransferFunction(scalarBarRange);
    scalarBarWidget->GetScalarBarActor()->SetLookupTable(colorTransferFunction);
    scalarBarWidget->SetEnabled(currentComponent == "Vector" ? layerProperties->getVectorsLegend() : layerProperties->getMapLegend());
    
    vtkScalarBarRepresentation *scalarBarRepresentation = vtkScalarBarRepresentation::SafeDownCast(scalarBarWidget->GetRepresentation());
    scalarBarRepresentation->GetPositionCoordinate()->SetValue(0.86, 0.05);
    scalarBarRepresentation->GetPosition2Coordinate()->SetValue(0.1, 0.35);
    
    scalarBarWidgets.insert(layerKey, scalarBarWidget);
    
    return scalarBarWidget;
}

vtkSmartPointer<vtkPolyData> SimulationVTKWidget::renderVectors() {
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
    
    vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetSourceConnection(arrowSource->GetOutputPort());
    glyph->SetInputData(arrowsPolyData);
    glyph->SetVectorModeToUseVector();
    glyph->SetScaleModeToScaleByVector();
    glyph->SetColorMode(layerProperties->getVectorColorMode() == VectorColorMode::MAGNITUDE ? VTK_COLOR_BY_VECTOR : VTK_COLOR_BY_SCALE);
    glyph->SetScaleFactor(layerProperties->getVectorsScale() * 1000);
    glyph->OrientOn();
    glyph->Update();
    
    glyph->SetRange(glyph->GetOutput()->GetPointData()->GetArray("VectorMagnitude")->GetRange());
    glyph->ClampingOn();
    glyph->Update();
    
    return vtkSmartPointer<vtkPolyData>(glyph->GetOutput());
}

vtkSmartPointer<vtkColorTransferFunction> SimulationVTKWidget::buildColorTransferFunction(double *scalarBarRange) {
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
            minimumRange = scalarBarRange[0];
            maximumRange = scalarBarRange[1];
        } else {
            minimumRange = layerProperties->getVectorsMinimumRange();
            maximumRange = layerProperties->getVectorsMaximumRange();
        }
    } else {
        colors = ColorGradientTemplate::getColors(layerProperties->getMapColorGradient());
        invertScalarBar = layerProperties->getMapInvertColorGradient();
        
        if (layerProperties->getUseDefaultMapValues()) {
            minimumRange = scalarBarRange[0];
            maximumRange = scalarBarRange[1];
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

void SimulationVTKWidget::hideLayer(const QString &layerKey) {
    QStringList layerAndComponent = layerKey.split("-");
    std::string layer = layerAndComponent.first().toStdString();
    std::string component = layerAndComponent.last().toStdString();
    
    if (component == "Vector") {
        vectorActors.value(layerKey)->VisibilityOff();
    } else {
        meshActor->GetProperty()->SetRepresentationToWireframe();
        layerDataSetMapper->ScalarVisibilityOff();
    }
    
    // Refactor
    scalarBarWidgets.value(layerKey)->EnabledOff();
    
    this->GetRenderWindow()->Render();
}

void SimulationVTKWidget::updateLayer() {
    this->render(currentSimulation, currentLayer, currentComponent, currentFrame);
}

void SimulationVTKWidget::removeLayer(const QString &layerKey) {
    QStringList layerAndComponent = layerKey.split("-");
    
    if (layerAndComponent.last() == "Vector") {
        vtkSmartPointer<vtkActor> vectorsActor = vectorActors.take(layerKey);
        this->renderer->RemoveActor(vectorsActor);
    } else {
        std::string arrayName = layerAndComponent.last() == "Magnitude" ? MAGNITUDE_ARRAY_NAME : layerAndComponent.first().toStdString();
        layerGrid->GetCellData()->RemoveArray(arrayName.c_str());
    }
    
    vtkSmartPointer<vtkScalarBarWidget> scalarBarWidget = scalarBarWidgets.take(layerKey);
    
    if (scalarBarWidget) {
        scalarBarWidget->EnabledOff();
    }
    
    this->GetRenderWindow()->Render();
}