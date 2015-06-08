#ifndef MESH_VTK_WIDGET_H
#define MESH_VTK_WIDGET_H

#include <QWidget>
#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkActor.h>

#include "include/domain/mesh.h"

class MeshVTKWidget : public QVTKWidget {
	Q_OBJECT
private:
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkActor> boundaryEdgesActor;
	vtkSmartPointer<vtkActor> gridActor;

public:
    MeshVTKWidget(QWidget *parent);
    ~MeshVTKWidget();
    void render(Mesh *mesh);
    void clear();
    void showBoundaryEdges(const bool &show);
    void showMesh(const bool &show);
};

#endif // MESH_VTK_WIDGET_H
