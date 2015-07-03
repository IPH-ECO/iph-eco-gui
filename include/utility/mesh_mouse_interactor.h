#ifndef MESH_MOUSE_INTERACTOR_H
#define MESH_MOUSE_INTERACTOR_H

#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkWorldPointPicker.h>
#include <vtkAbstractPicker.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <QObject>

class MeshMouseInteractor : public QObject, public vtkInteractorStyleRubberBandPick {
    Q_OBJECT
public:
    static MeshMouseInteractor* New();
    vtkTypeMacro(MeshMouseInteractor, vtkInteractorStyleRubberBandPick);

    virtual void OnMouseMove() {
        vtkSmartPointer<vtkWorldPointPicker> picker = vtkSmartPointer<vtkWorldPointPicker>::New();
        int *mousePosition = this->Interactor->GetEventPosition();
        
        picker->Pick(mousePosition[0], mousePosition[1], 0, this->GetDefaultRenderer());
        
        double* worldPosition = picker->GetPickPosition();
        
        emit coordinateChanged(worldPosition[0], worldPosition[1]);
        
        vtkInteractorStyleRubberBandPick::OnMouseMove();
    }

signals:
    void coordinateChanged(double &x, double &y);
};

#endif // MESH_MOUSE_INTERACTOR_H
