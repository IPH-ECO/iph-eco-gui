#ifndef UNSTRUCTURED_MESH_VTK_WIDGET_H
#define UNSTRUCTURED_MESH_VTK_WIDGET_H

#include <QWidget>
#include <QVTKWidget.h>
#include <vtkAutoInit.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>

#include "include/domain/unstructured_mesh.h"

class UnstructuredMeshVTKWidget : public QVTKWidget, public vtkInteractorStyleTrackballCamera {
private:
//    vtkPolyDataMapper *polyDataMapper;
//    vtkActor *actor;
//    vtkRenderWindow *renderWindow;
//    vtkRenderer *renderer;
public:
    static UnstructuredMeshVTKWidget* New();
    vtkTypeMacro(UnstructuredMeshVTKWidget, vtkInteractorStyleTrackballCamera)

    UnstructuredMeshVTKWidget() {}
    UnstructuredMeshVTKWidget(QWidget *parent);
    ~UnstructuredMeshVTKWidget();
    void setMesh(UnstructuredMesh *mesh);

    virtual void OnMouseClick();
};

#endif // UNSTRUCTURED_MESH_VTK_WIDGET_H
