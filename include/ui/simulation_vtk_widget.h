#ifndef SIMULATION_VTK_WIDGET_H
#define SIMULATION_VTK_WIDGET_H

#include "include/domain/simulation.h"
#include "include/ui/mesh_vtk_widget.h"
#include "include/domain/meteorological_station.h"

#include <QFileInfo>
#include <vtkDoubleArray.h>
#include <vtkDataSetMapper.h>
#include <vtkScalarBarWidget.h>
#include <vtkUnstructuredGrid.h>
#include <vtkColorTransferFunction.h>

class SimulationVTKWidget : public MeshVTKWidget {
	Q_OBJECT
private:
    vtkSmartPointer<vtkUnstructuredGrid> layerGrid;
    vtkSmartPointer<vtkActor> layerActor;
    vtkSmartPointer<vtkDataSetMapper> layerDataSetMapper;
    QMap<QString, vtkSmartPointer<vtkActor> > vectorsActors;
    QMap<QString, vtkSmartPointer<vtkScalarBarWidget> > scalarBarWidgets;
    Simulation *currentSimulation;
    QFileInfoList outputFiles;
    LayerProperties *layerProperties;
    QString currentLayer;
    QString currentComponent;
    int currentFrame;
    
    const char *MAGNITUDE_ARRAY_NAME;
    
    void renderMeshLayer();
    void readFrame(const int frame);
    vtkSmartPointer<vtkDoubleArray> buildMagnitudeArray();
    vtkSmartPointer<vtkColorTransferFunction> buildColorTransferFunction(double *scalarBarRange);
    vtkSmartPointer<vtkPolyData> renderVectors();
    vtkSmartPointer<vtkScalarBarWidget> renderScalarBar(const QString &layerKey, double *scalarBarRange);
public:
	explicit SimulationVTKWidget(QWidget *parent);
	void render(Simulation *simulation, const QString &layer, const QString &component, int frame);
    void removeLayer(const QString &layerKey);
public slots:
    void hideLayer(const QString &layerKey);
    void updateLayer();
    virtual void changeMeshProperties(Mesh *mesh);
};

#endif // SIMULATION_VTK_WIDGET_H