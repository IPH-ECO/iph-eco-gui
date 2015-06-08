#ifndef STRUCTURED_MESH_VTK_WIDGET_H
#define STRUCTURED_MESH_VTK_WIDGET_H

#include <QWidget>
#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkActor.h>

#include "include/domain/mesh.h"

class StructuredMeshVTKWidget : public QVTKWidget {
	Q_OBJECT
private:
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkActor> boundaryEdgesActor;
	vtkSmartPointer<vtkActor> gridActor;

public:
    StructuredMeshVTKWidget(QWidget *parent);
    ~StructuredMeshVTKWidget();
    void render(Mesh *mesh);
    void clear();
    void showBoundaryEdges(const bool &show);
    void showMesh(const bool &show);
};

#endif // STRUCTURED_MESH_VTK_WIDGET_H
