#ifndef MESH_MOUSE_INTERACTOR_H
#define MESH_MOUSE_INTERACTOR_H

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkAbstractPicker.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <QObject>

class MeshMouseInteractor : public QObject, public vtkInteractorStyleTrackballCamera {
    Q_OBJECT
public:
    static MeshMouseInteractor* New();
    vtkTypeMacro(MeshMouseInteractor, vtkInteractorStyleTrackballCamera);

    virtual void OnMouseMove() {
        int* mousePosition = this->Interactor->GetEventPosition();
        double worldPosition[3];

        this->Interactor->GetPicker()->Pick(mousePosition[0], mousePosition[1], 0, this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
        this->Interactor->GetPicker()->GetPickPosition(worldPosition);

        emit coordinateChanged(worldPosition[0], worldPosition[1]);

        vtkInteractorStyleTrackballCamera::OnMouseMove();
    }

signals:
    void coordinateChanged(double &x, double &y);
};

#endif // MESH_MOUSE_INTERACTOR_H
