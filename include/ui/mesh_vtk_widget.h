#ifndef MESH_VTK_WIDGET_H
#define MESH_VTK_WIDGET_H

#include <QWidget>
#include <vtkActor.h>
#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>
#include <vtkCubeAxesActor.h>

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
};

#endif // MESH_VTK_WIDGET_H
