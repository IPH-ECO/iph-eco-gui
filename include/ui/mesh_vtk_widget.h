#ifndef MESH_VTK_WIDGET_H
#define MESH_VTK_WIDGET_H

#include <QWidget>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>
#include <vtkCubeAxesActor.h>
#include <vtkPolyDataMapper.h>

#include "include/domain/mesh.h"
#include "include/utility/mesh_mouse_interactor.h"

class MeshVTKWidget : public QVTKWidget {
	Q_OBJECT
private:
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkSmartPointer<MeshMouseInteractor> mouseInteractor;
    
    vtkSmartPointer<vtkActor> meshActor;
    vtkSmartPointer<vtkCubeAxesActor> axesActor;
	vtkSmartPointer<vtkActor> boundaryEdgesActor;
    vtkSmartPointer<vtkActor2D> labelsActor;
    vtkSmartPointer<vtkActor> verticesActor;
    
    Mesh *currentMesh;
    bool showBoundaryEdges;
    bool showMesh;
    bool showAxes;

public:
    MeshVTKWidget(QWidget *parent);
    ~MeshVTKWidget();
    void render(Mesh *mesh);
    void clear();

public slots:
    void toggleBoundaryEdges(bool show);
    void toggleMesh(bool show);
    void toggleAxes(bool show);
    void resetZoom();
    void toggleZoomArea(bool activate);
    void toggleLabels(const LabelType &labelType);
    void changeBackgroundColor(const double &r, const double &g, const double &b);
    void exportToImage(const QString &fileName);
};

#endif // MESH_VTK_WIDGET_H
