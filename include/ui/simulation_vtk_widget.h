#ifndef SIMULATION_VTK_WIDGET_H
#define SIMULATION_VTK_WIDGET_H

#include "include/domain/simulation.h"
#include "include/ui/mesh_vtk_widget.h"
#include "include/domain/meteorological_station.h"

#include <vtkScalarBarWidget.h>
#include <vtkUnstructuredGrid.h>
#include <vtkColorTransferFunction.h>

class SimulationVTKWidget : public MeshVTKWidget {
	Q_OBJECT
private:
    QMap<QString, vtkSmartPointer<vtkActor> > layerActors;
    QMap<vtkSmartPointer<vtkActor>, vtkSmartPointer<vtkScalarBarWidget> > scalarBarWidgets;
    Simulation *currentSimulation;
    LayerProperties *layerProperties;
    QString currentLayer;
    QString currentComponent;
    int currentFrame;
    
    const char *MAGNITUDE_ARRAY_NAME;
    
    vtkSmartPointer<vtkColorTransferFunction> buildColorTransferFunction(double *scalarBarRange);
    vtkSmartPointer<vtkPolyData> renderVectors(vtkSmartPointer<vtkUnstructuredGrid> layerGrid);
    vtkSmartPointer<vtkUnstructuredGrid> convertToMagnitudeGrid(vtkSmartPointer<vtkUnstructuredGrid> layerGrid);
    vtkSmartPointer<vtkScalarBarWidget> renderScalarBar(vtkSmartPointer<vtkActor> layerActor, double *scalarBarRange);
public:
	explicit SimulationVTKWidget(QWidget *parent);
	void render(Simulation *simulation, const QString &layer, const QString &component, int frame);
    void removeLayer(const QString &layerKey);
public slots:
    void toggleLayerVisibility(const QString &layerKey, bool visible);
    void updateLayer();
};

#endif // SIMULATION_VTK_WIDGET_H