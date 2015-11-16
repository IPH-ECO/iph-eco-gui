#include "include/ui/simulation_vtk_widget.h"

#include "include/ui/main_window.h"
#include "include/exceptions/simulation_exception.h"

#include <QFileInfo>
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph2D.h>
#include <vtkLookupTable.h>
#include <vtkDoubleArray.h>
#include <vtkArrowSource.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkColorTransferFunction.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkGenericGlyph3DFilter.h>

SimulationVTKWidget::SimulationVTKWidget(QWidget *parent) :
    MeshVTKWidget(parent), currentSimulation(nullptr), layerProperties(nullptr), showSurface(true)
{}

void SimulationVTKWidget::render(Simulation *simulation, const QString &layer, const QString &component, int frame) {
    currentSimulation = simulation;
    layerProperties = simulation->getSelectedLayers().value(QString("%1-%2").arg(layer).arg(component));
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
        
        bool isVector = array->GetNumberOfComponents() == 3;
        
        vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
        vtkSmartPointer<vtkColorTransferFunction> pointsColorTransferFunction = buildColorTransferFunction(true);
        
        if (isVector) {
            vtkSmartPointer<vtkPolyData> vectorPolyData = vtkSmartPointer<vtkPolyData>::New();
            vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
            
            for (vtkIdType i = 0; i < array->GetNumberOfTuples(); i++) {
                points->InsertNextPoint(array->GetTuple(i));
            }
            
            vectorPolyData->SetPoints(points);
            
            vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
            
            vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
            glyph->SetSourceData(vectorPolyData);
            glyph->SetInputConnection(arrowSource->GetOutputPort());
            glyph->ScalingOn();
            glyph->SetVectorModeToUseVector();
            glyph->OrientOn();
            glyph->Update();
            
            mapper->SetInputConnection(glyph->GetOutputPort());
            mapper->Update();
        } else {
            mapper->SetScalarModeToUseCellData();
            mapper->SetInputData(unstructuredGrid);
            mapper->SelectColorArray(arrayName.c_str());
        }
        
        mapper->SetLookupTable(pointsColorTransferFunction);
        mapper->UseLookupTableScalarRangeOn();
        
        mapActor = vtkSmartPointer<vtkActor>::New();
        mapActor->SetMapper(mapper);
        mapActor->GetProperty()->SetOpacity(layerProperties->getMapOpacity() / 100.0);
        mapActor->GetProperty()->SetLighting(layerProperties->getMapLighting());
        
        renderer->AddActor(mapActor);
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
    renderer->RemoveActor(mapActor);
    this->update();
}