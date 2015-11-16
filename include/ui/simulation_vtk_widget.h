#ifndef SIMULATION_VTK_WIDGET_H
#define SIMULATION_VTK_WIDGET_H

#include "include/domain/simulation.h"
#include "include/ui/mesh_vtk_widget.h"
#include "include/domain/meteorological_station.h"

#include <vtkScalarBarActor.h>
#include <vtkColorTransferFunction.h>

class SimulationVTKWidget : public MeshVTKWidget {
	Q_OBJECT
private:
    vtkSmartPointer<vtkActor> mapActor;
    vtkSmartPointer<vtkScalarBarActor> mapBarActor;
    vtkSmartPointer<vtkScalarBarActor> mapPointsBarActor;
    Simulation *currentSimulation;
    LayerProperties *layerProperties;
    QString currentLayer;
    QString currentComponent;
    int currentFrame;
    bool showSurface;
    
    vtkSmartPointer<vtkColorTransferFunction> buildColorTransferFunction(bool isColorMap);
public:
	explicit SimulationVTKWidget(QWidget *parent);
	void render(Simulation *simulation, const QString &layer, const QString &component, int frame);
    virtual void clear();
public slots:
    void toggleRepresentation(bool showSurface);
    void updateLayer();
};

#endif // SIMULATION_VTK_WIDGET_H