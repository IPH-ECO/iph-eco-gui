#ifndef HYDRODYNAMIC_VTK_WIDGET_H
#define HYDRODYNAMIC_VTK_WIDGET_H

#include "include/domain/mesh.h"
#include "include/utility/mesh_mouse_interactor.h"

#include <QVTKWidget.h>
#include <vtkActor2D.h>
#include <vtkIdTypeArray.h>
#include <vtkCubeAxesActor.h>
#include <vtkWorldPointPicker.h>
#include <vtkRenderWindowInteractor.h>

class HydrodynamicVTKWidget : public QVTKWidget {
private:
	vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkSmartPointer<MeshMouseInteractor> mouseInteractor;

    vtkSmartPointer<vtkActor> meshActor;
    vtkSmartPointer<vtkCubeAxesActor> axesActor;
    vtkSmartPointer<vtkActor2D> labelsActor;

    vtkSmartPointer<vtkIdTypeArray> selectedCellIds;

    Mesh *currentMesh;
    
    bool showAxes;

public:
    explicit HydrodynamicVTKWidget(QWidget *parent);
	void render(Mesh *mesh);
    void toggleAxes(bool show);
};

#endif // HYDRODYNAMIC_VTK_WIDGET_H