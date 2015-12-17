#ifndef SIMULATION_VTK_WIDGET_H
#define SIMULATION_VTK_WIDGET_H

#include "include/domain/simulation.h"
#include "include/ui/mesh_vtk_widget.h"
#include "include/domain/meteorological_station.h"

#include <QFileInfo>
#include <vtkTextActor.h>
#include <vtkDoubleArray.h>
#include <vtkDataSetMapper.h>
#include <vtkScalarBarActor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkColorTransferFunction.h>

class SimulationVTKWidget : public MeshVTKWidget {
	Q_OBJECT
private:
    const char *MAGNITUDE_ARRAY_NAME;
    
    vtkSmartPointer<vtkUnstructuredGrid> layerGrid;
    vtkSmartPointer<vtkActor> layerActor;
    vtkSmartPointer<vtkDataSetMapper> layerDataSetMapper;
    QMap<QString, vtkSmartPointer<vtkActor> > vectorsActors;
    QMap<QString, vtkSmartPointer<vtkScalarBarActor> > scalarBarActors;
    vtkSmartPointer<vtkTextActor> timeStampActor;
    QList<QString> visibleScalarBarActors;
    Simulation *currentSimulation;
    QFileInfoList outputFiles;
    LayerProperties *layerProperties;
    QString currentLayer;
    QString currentComponent;
    int currentFrame;
    QString axesScale;
    
    void renderMeshLayer();
    std::string readFrame(const int frame);
    vtkSmartPointer<vtkDoubleArray> buildMagnitudeArray();
    vtkSmartPointer<vtkColorTransferFunction> buildColorTransferFunction(double *scalarBarRange);
    vtkSmartPointer<vtkPolyData> renderVectors();
    vtkSmartPointer<vtkScalarBarActor> renderScalarBar(const QString &layerKey, double *scalarBarRange);
public:
	explicit SimulationVTKWidget(QWidget *parent);
	void render(Simulation *simulation, const QString &layer, const QString &component, int frame);
    void removeLayer(const QString &layerKey);
    void setAxesScale(const QString &axesScale);
    void updateOutputFileList();
    virtual void clear();
public slots:
    void hideLayer(const QString &layerKey);
    void updateLayer();
    virtual void changeMeshProperties(Mesh *mesh);
    void exportAnimationToVideo();
};

#endif // SIMULATION_VTK_WIDGET_H