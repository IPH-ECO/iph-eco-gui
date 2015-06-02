#ifndef MOUSE_INTERACTOR_H
#define MOUSE_INTERACTOR_H

#include <vtkSmartPointer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkPropPicker.h>
#include <QDebug>

class MouseInteractor : public QObject, public vtkInteractorStyleTrackballCamera {
    Q_OBJECT
public:
    static MouseInteractor* New();
    vtkTypeMacro(MouseInteractor, vtkInteractorStyleTrackballCamera);

    virtual void OnMouseMove() {
        int* mousePosition = this->GetInteractor()->GetEventPosition();

        vtkSmartPointer<vtkPropPicker> picker = vtkSmartPointer<vtkPropPicker>::New();

        picker->Pick(mousePosition[0], mousePosition[1], 0, this->GetDefaultRenderer());

        double *worldPosition = picker->GetPickPosition();

        emit coordinateChanged(worldPosition[0], worldPosition[1]);
    }

signals:
    void coordinateChanged(double &x, double &y);
};

#endif // MOUSE_INTERACTOR_H
