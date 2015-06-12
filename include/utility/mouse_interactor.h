#ifndef MOUSE_INTERACTOR_H
#define MOUSE_INTERACTOR_H

#include <vtkSmartPointer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkRendererCollection.h>
#include <vtkAbstractPicker.h>
#include <vtkRenderWindow.h>

class MouseInteractor : public QObject, public vtkInteractorStyleTrackballCamera {
    Q_OBJECT
public:
    static MouseInteractor* New();
    vtkTypeMacro(MouseInteractor, vtkInteractorStyleTrackballCamera);

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

#endif // MOUSE_INTERACTOR_H
