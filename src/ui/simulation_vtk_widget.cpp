#include "include/ui/simulation_vtk_widget.h"

#include "include/ui/main_window.h"
#include "include/exceptions/simulation_exception.h"

#include <vtkGlyph3D.h>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkTransform.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkDoubleArray.h>
#include <vtkArrowSource.h>
#include <vtkCellCenters.h>
#include <vtkPolyDataMapper.h>
#include <vtkScalarBarActor.h>
#include <vtkArrayCalculator.h>
#include <vtkColorTransferFunction.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkScalarBarRepresentation.h>
#include <vtkGenericDataObjectReader.h>

SimulationVTKWidget::SimulationVTKWidget(QWidget *parent) :
    MeshVTKWidget(parent),
    currentSimulation(nullptr),
    layerProperties(nullptr),
    axesScale("1 1 1"),
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
        
        renderMeshLayer();
        
        renderer->RemoveActor(layerActor);
        
        layerDataSetMapper = vtkSmartPointer<vtkDataSetMapper>::New();
        layerDataSetMapper->SetInputData(layerGrid);
        layerDataSetMapper->ScalarVisibilityOff();
        
        layerActor = vtkSmartPointer<vtkActor>::New();
        layerActor->GetProperty()->EdgeVisibilityOff();
        layerActor->SetScale(meshActor->GetScale());
        layerActor->SetMapper(layerDataSetMapper);
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
    
    std::string layerArrayName = layer.toStdString();
    double layerRange[2];
    
    if (component == "Magnitude") {
        vtkSmartPointer<vtkDoubleArray> layerArray = this->buildMagnitudeArray();
        
        layerArray->GetRange(layerRange);
        layerGrid->GetCellData()->AddArray(layerArray);
    } else if (component == "Vector") {
        layerArrayName = MAGNITUDE_ARRAY_NAME;
    } else { // Arbritary layer
        layerGrid->GetCellData()->GetArray(layerArrayName.c_str())->GetRange(layerRange);
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
        
        if (vectorsActors.value(layerKey)) {
            renderer->RemoveActor(vectorsActors.take(layerKey));
        }
        
        vectorsActors.insert(layerKey, vectorsActor);
        renderer->AddActor(vectorsActor);
        
        vectorsActor->GetProperty()->SetOpacity(layerProperties->getVectorsOpacity() / 100.0);
        
        if (layerProperties->getVectorColorMode() == VectorColorMode::CONSTANT) {
            QColor vectorColor = layerProperties->getVectorsColor();
            vectorsActor->GetProperty()->SetColor(vectorColor.redF(), vectorColor.greenF(), vectorColor.blueF());
        }
    } else {
        vtkSmartPointer<vtkScalarBarWidget> scalarBarWidget = renderScalarBar(layerKey, layerRange);
        
        layerDataSetMapper->SetInputData(layerGrid);
        layerDataSetMapper->SetLookupTable(scalarBarWidget->GetScalarBarActor()->GetLookupTable());
        layerDataSetMapper->SelectColorArray(layerArrayName.c_str());
        layerDataSetMapper->UseLookupTableScalarRangeOn();
        layerDataSetMapper->SetScalarModeToUseCellData();
        layerDataSetMapper->ScalarVisibilityOn();
        
        layerActor->SetMapper(layerDataSetMapper);
        layerActor->GetProperty()->SetOpacity(layerProperties->getMapOpacity() / 100.0);
        layerActor->GetProperty()->SetLighting(layerProperties->getMapLighting());
        layerActor->GetProperty()->SetRepresentationToSurface();
        layerActor->VisibilityOn();
        
        renderer->AddActor(layerActor);
    }
    
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
    
    vtkSmartPointer<vtkDataSetMapper> meshMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    meshMapper->SetResolveCoincidentTopologyToShiftZBuffer();
    meshMapper->SetInputData(layerGrid);
    meshMapper->ScalarVisibilityOff();
    
    this->renderer->RemoveActor(meshActor);
    
    QColor meshColor(currentMesh->getColor());
    QStringList scales = axesScale.split(" ");
    
    meshActor = vtkSmartPointer<vtkActor>::New();
    meshActor->SetMapper(meshMapper);
    meshActor->GetProperty()->SetRepresentationToWireframe();
    meshActor->GetProperty()->LightingOff();
    meshActor->SetScale(scales[0].toInt(), scales[1].toInt(), scales[2].toInt());
    this->changeMeshProperties(currentMesh);
    
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
    magnitudeFunction->SetResultArrayName(MAGNITUDE_ARRAY_NAME);
    magnitudeFunction->SetFunction("sqrt(x^2 + y^2 + z^2)");
    magnitudeFunction->SetAttributeModeToUseCellData();
    magnitudeFunction->SetInputData(layerGrid);
    magnitudeFunction->Update();
    
    vtkSmartPointer<vtkUnstructuredGrid> magnitudePolyData = magnitudeFunction->GetUnstructuredGridOutput();
    
    return vtkSmartPointer<vtkDoubleArray>(vtkDoubleArray::SafeDownCast(magnitudePolyData->GetCellData()->GetArray(MAGNITUDE_ARRAY_NAME)));
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
    
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Scale(meshActor->GetScale());
    
    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInputData(arrowsPolyData);
    transformFilter->SetTransform(transform);
    transformFilter->Update();
    
    // Tip: > Shaft: --
    vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
    arrowSource->SetShaftResolution(25);
    arrowSource->SetTipResolution(25);
    arrowSource->SetTipLength(0.2);
    arrowSource->SetShaftRadius(layerProperties->getVectorsWidth() * 0.015); // VTK default
    arrowSource->SetTipRadius(layerProperties->getVectorsWidth() * 0.05); // VTK default
    arrowSource->Update();
    
    vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetSourceConnection(arrowSource->GetOutputPort());
    glyph->SetInputData(transformFilter->GetOutput());
    glyph->SetVectorModeToUseVector();
    glyph->SetScaleModeToScaleByVector();
    glyph->SetColorMode(layerProperties->getVectorColorMode() == VectorColorMode::MAGNITUDE ? VTK_COLOR_BY_VECTOR : VTK_COLOR_BY_SCALE);
    glyph->SetScaleFactor(layerProperties->getVectorsScale() * 1000);
    glyph->OrientOn();
    glyph->Update();
    
    glyph->SetRange(glyph->GetOutput()->GetPointData()->GetArray(MAGNITUDE_ARRAY_NAME)->GetRange());
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
        
        minimumRange = layerProperties->getUseCustomVectorsMinimum() ? layerProperties->getCustomVectorsMinimumRange() : scalarBarRange[0];
        maximumRange = layerProperties->getUseCustomVectorsMaximum() ? layerProperties->getCustomVectorsMaximumRange() : scalarBarRange[1];
    } else {
        colors = ColorGradientTemplate::getColors(layerProperties->getMapColorGradient());
        invertScalarBar = layerProperties->getMapInvertColorGradient();
        minimumRange = layerProperties->getUseCustomMapMinimum() ? layerProperties->getCustomMapMininumRange() : scalarBarRange[0];
        maximumRange = layerProperties->getUseCustomMapMaximum() ? layerProperties->getCustomMapMaximumRange() : scalarBarRange[1];
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
        vectorsActors.value(layerKey)->VisibilityOff();
    } else {
        layerActor->VisibilityOff();
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
        this->renderer->RemoveActor(vectorsActors.take(layerKey));
    } else if (layerAndComponent.first() == layerDataSetMapper->GetArrayName()) {
        layerActor->VisibilityOff();
    }
    
    vtkSmartPointer<vtkScalarBarWidget> scalarBarWidget = scalarBarWidgets.take(layerKey);
    
    if (scalarBarWidget) {
        scalarBarWidget->EnabledOff();
    }
    
    this->GetRenderWindow()->Render();
}

void SimulationVTKWidget::setAxesScale(const QString &axesScale) {
    QStringList scales = axesScale.split(" ");
    int xScale = scales[0].toInt();
    int yScale = scales[1].toInt();
    int zScale = scales[2].toInt();
    
    this->axesScale = axesScale;
    
    if (meshActor) {
        this->meshActor->SetScale(xScale, yScale, zScale);
        this->axesActor->SetBounds(meshActor->GetBounds());
        this->layerActor->SetScale(xScale, yScale, zScale);
    }
    
    if (!vectorsActors.isEmpty()) {
        for (vtkSmartPointer<vtkActor> vectorsActor : vectorsActors.values()) {
            vtkSmartPointer<vtkPolyData> vectorsPolyData = renderVectors();
            vtkSmartPointer<vtkPolyDataMapper> vectorsPolyDataMapper = vtkPolyDataMapper::SafeDownCast(vectorsActor->GetMapper());
            vectorsPolyDataMapper->SetInputData(vectorsPolyData);
        }
    }
    
    this->renderer->ResetCamera();
    this->GetRenderWindow()->Render();
}

void SimulationVTKWidget::changeMeshProperties(Mesh *mesh) {
    if (meshActor) {
        QColor meshColor(mesh->getColor());
        
        meshActor->GetProperty()->SetColor(meshColor.redF(), meshColor.greenF(), meshColor.blueF());
        meshActor->GetProperty()->SetLineStipplePattern(mesh->getLineStyle());
        meshActor->GetProperty()->SetLineWidth(mesh->getLineWidth());
        meshActor->GetProperty()->SetOpacity(mesh->getOpacity() / 100.0);
        
        this->GetRenderWindow()->Render();
    }
}