#include "include/ui/simulation_vtk_widget.h"

#include "include/ui/main_window.h"
#include "include/exceptions/simulation_exception.h"

#include <QFileInfo>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkColorTransferFunction.h>
#include <vtkGenericDataObjectReader.h>

SimulationVTKWidget::SimulationVTKWidget(QWidget *parent) :
    MeshVTKWidget(parent), currentSimulation(nullptr), layerProperties(nullptr), showSurface(true)
{}

void SimulationVTKWidget::render(Simulation *simulation, const QString &layer, const QString &component, int frame) {
    currentSimulation = simulation;
    layerProperties = simulation->getSelectedLayers().value(QString("%1-%2").arg(layer).arg(component));
    currentLayer = layer;
    currentComponent = component;
    currentFrame = frame;
    
    if (layer.isEmpty()) {
        MeshVTKWidget::render(simulation->getMesh());
    } else {
        QFileInfoList outputFiles = simulation->getOutputFiles();
        
        if (frame >= outputFiles.size()) {
            throw SimulationException("Layer out of range.");
        }
        
        clear();
        
        std::string filename = outputFiles.at(frame).absoluteFilePath().toStdString();
        std::string arrayName = layer.toStdString();
        
        vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
        reader->SetFileName(filename.c_str());
        reader->Update();
        
        vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid = reader->GetUnstructuredGridOutput();
        double *range = unstructuredGrid->GetCellData()->GetArray(arrayName.c_str())->GetRange();
        layerProperties->setMapMinimumRange(range[0]);
        layerProperties->setMapMaximumRange(range[1]);
        layerProperties->setPointsMinimumRange(range[0]);
        layerProperties->setPointsMaximumRange(range[1]);
        
        vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
        vtkSmartPointer<vtkColorTransferFunction> pointsColorTransferFunction = buildColorTransferFunction(true);
        
        mapper->SetScalarModeToUseCellData();
        mapper->UseLookupTableScalarRangeOn();
        mapper->SetInputData(unstructuredGrid);
        mapper->SelectColorArray(arrayName.c_str());
        mapper->SetLookupTable(pointsColorTransferFunction);
        
        simulationActor = vtkSmartPointer<vtkActor>::New();
        simulationActor->SetMapper(mapper);

        renderer->AddActor(simulationActor);
        renderer->ResetCamera();
        
        MainWindow *mainWindow = static_cast<MainWindow*>(this->topLevelWidget());
        QObject::connect(mouseInteractor, SIGNAL(coordinateChanged(double&, double&)), mainWindow, SLOT(setCoordinate(double&, double&)));
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