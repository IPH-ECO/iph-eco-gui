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
#include <vtkUnstructuredGrid.h>
#include <vtkColorTransferFunction.h>
#include <vtkScalarBarRepresentation.h>
#include <vtkGenericDataObjectReader.h>

SimulationVTKWidget::SimulationVTKWidget(QWidget *parent) :
    MeshVTKWidget(parent), currentSimulation(nullptr), layerProperties(nullptr), showSurface(true)
{}

void SimulationVTKWidget::render(Simulation *simulation, const QString &layer, const QString &component, int frame) {
    QString layerKey = QString("%1-%2").arg(layer).arg(component);
    
    currentSimulation = simulation;
    layerProperties = simulation->getSelectedLayers().value(layerKey);
    currentLayer = layer;
    currentComponent = component;
    currentFrame = frame;
    
    clear();
    
    if (layer.isEmpty()) {
        MeshVTKWidget::render(simulation->getMesh());
    } else {
        QFileInfoList outputFiles = simulation->getOutputFiles();
        
        if (frame >= outputFiles.size()) {
            throw SimulationException("Layer out of range.");
        }
        
        std::string filename = outputFiles.at(frame).absoluteFilePath().toStdString();
        std::string arrayName = layer.toStdString();
        
        vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
        reader->SetFileName(filename.c_str());
        reader->Update();
        
        vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid = reader->GetUnstructuredGridOutput();
        vtkSmartPointer<vtkDoubleArray> array = vtkDoubleArray::SafeDownCast(unstructuredGrid->GetCellData()->GetArray(arrayName.c_str()));
        double *range = array->GetRange();
        
        layerProperties->setMapMinimumRange(range[0]);
        layerProperties->setMapMaximumRange(range[1]);
        layerProperties->setPointsMinimumRange(range[0]);
        layerProperties->setPointsMaximumRange(range[1]);
        
        vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
        actor->GetProperty()->SetOpacity(layerProperties->getMapOpacity() / 100.0);
        actor->GetProperty()->SetLighting(layerProperties->getMapLighting());
        
        if (array->GetNumberOfComponents() == 3) { // Is vector
            vtkSmartPointer<vtkCellCenters> cellCentersFilter = vtkSmartPointer<vtkCellCenters>::New();
            cellCentersFilter->SetInputData(unstructuredGrid);
            cellCentersFilter->Update();
            
            vtkSmartPointer<vtkPolyData> vectorPolyData = vtkSmartPointer<vtkPolyData>::New();
            vectorPolyData->SetPoints(cellCentersFilter->GetOutput()->GetPoints());
            vectorPolyData->GetPointData()->SetVectors(array);
            
            vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
            vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
            glyph->SetSourceConnection(arrowSource->GetOutputPort());
            glyph->SetInputData(vectorPolyData);
            glyph->SetVectorModeToUseVector();
            glyph->SetScaleModeToScaleByVector();
            glyph->SetColorModeToColorByScale();
            glyph->SetScaleFactor(10000);
            glyph->OrientOn();
            glyph->Update();
            
            vtkSmartPointer<vtkPolyDataMapper> mapMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapMapper->SetInputConnection(glyph->GetOutputPort());
            mapMapper->Update();
            
            actor->SetMapper(mapMapper);
        } else {
            if (mapScalarBarWidget) {
                mapScalarBarWidget->EnabledOff();
                renderer->RemoveActor(mapScalarBarWidget->GetScalarBarActor());
            }
            
            vtkSmartPointer<vtkColorTransferFunction> mapColorTransferFunction = buildColorTransferFunction(true);
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
            
            vtkScalarBarRepresentation *scalarBarRepresentation = vtkScalarBarRepresentation::SafeDownCast(mapScalarBarWidget->GetRepresentation());
            scalarBarRepresentation->GetPositionCoordinate()->SetValue(0.86, 0.05);
            scalarBarRepresentation->GetPosition2Coordinate()->SetValue(0.1, 0.35);
            
            vtkSmartPointer<vtkDataSetMapper> mapMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            mapMapper->SetScalarModeToUseCellData();
            mapMapper->SetInputData(unstructuredGrid);
            mapMapper->SelectColorArray(arrayName.c_str());
            mapMapper->SetLookupTable(mapColorTransferFunction);
            mapMapper->UseLookupTableScalarRangeOn();
            
            actor->SetMapper(mapMapper);
        }
        
        layerActors.insert(layerKey, actor);
        renderer->AddActor(actor);
        renderer->ResetCamera();
    }
}

vtkSmartPointer<vtkColorTransferFunction> SimulationVTKWidget::buildColorTransferFunction(bool showSurface) {
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    QList<QColor> colors;
    bool invertScalarBar;
    double minimumRange;
    double maximumRange;
    double interval;
    
    if (showSurface) {
        colors = ColorGradientTemplate::getColors(layerProperties->getMapColorGradient());
        invertScalarBar = layerProperties->getMapInvertColorGradient();
        minimumRange = layerProperties->getMapMininumRange();
        maximumRange = layerProperties->getMapMaximumRange();
        interval = layerProperties->getMapMaximumRange() - layerProperties->getMapMininumRange();
    } else {
        colors = ColorGradientTemplate::getColors(layerProperties->getPointsColorGradient());
        invertScalarBar = layerProperties->getPointsInvertColorGradient();
        minimumRange = layerProperties->getPointsMininumRange();
        maximumRange = layerProperties->getPointsMaximumRange();
        interval = layerProperties->getPointsMaximumRange() - layerProperties->getPointsMininumRange();
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

void SimulationVTKWidget::toggleRepresentation(bool showSurface) {
}

void SimulationVTKWidget::updateLayer() {
    this->render(currentSimulation, currentLayer, currentComponent, currentFrame);
}

void SimulationVTKWidget::clear() {
    this->update();
}