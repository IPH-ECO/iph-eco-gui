#ifndef MESH_MOUSE_INTERACTOR_H
#define MESH_MOUSE_INTERACTOR_H

#include <application/iph_types.h>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleRubberBandPick.h>

class MeshMouseInteractor : public vtkInteractorStyleRubberBandPick {
protected:
    PickerMode pickerMode;
    double worldCoordinate[2];
    vtkIdType lastCellId;
public:
    static MeshMouseInteractor* New();
    vtkTypeMacro(MeshMouseInteractor, vtkInteractorStyleRubberBandPick);
    
    MeshMouseInteractor();

    virtual void OnLeftButtonDown();
    virtual void OnMouseMove();
    virtual void activatePicker(const PickerMode &pickerMode);
    virtual void deactivatePicker();

    PickerMode getPickerMode() const;
};

#endif // MESH_MOUSE_INTERACTOR_H
