#ifndef SIMULATION_VTK_WIDGET_H
#define SIMULATION_VTK_WIDGET_H

#include <domain/simulation.h>
#include <domain/meteorological_station.h>
#include <ui/mesh_vtk_widget.h>

#include <QFileInfo>
#include <QMouseEvent>
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
    bool cancelExportVideoOperation;
    
    void renderMeshLayer();
    std::string readFrame(const int frame);
    vtkSmartPointer<vtkDoubleArray> buildMagnitudeArray();
    vtkSmartPointer<vtkColorTransferFunction> buildColorTransferFunction(double *scalarBarRange);
    vtkSmartPointer<vtkPolyData> renderVectors();
    vtkSmartPointer<vtkScalarBarActor> renderScalarBar(const QString &layerKey, double *scalarBarRange);
private slots:
    void handleMouseEvent(QMouseEvent *event);
public:
	explicit SimulationVTKWidget(QWidget *parent);
	void render(Simulation *simulation, const QString &layer, const QString &component, int frame);
    void removeLayer(const QString &layerKey);
    void setAxesScale(const QString &axesScale);
    void updateOutputFileList();
    int getNumberOfMapLayers() const;
    void togglePicker(bool activate);
    virtual void clear();
public slots:
    void hideLayer(const QString &layerKey);
    void updateLayer();
    virtual void changeMeshProperties(Mesh *mesh);
    void exportVideo(int initialFrame, int finalFrame, int frameStep, int frameRate, const QString &outputFile);
    void cancelExportVideo();
signals:
    void updateExportVideoProgress(int progress);
};

#endif // SIMULATION_VTK_WIDGET_H