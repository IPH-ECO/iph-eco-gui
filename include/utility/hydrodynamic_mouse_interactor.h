#ifndef HYDRODYNAMIC_MOUSE_INTERACTOR_H
#define HYDRODYNAMIC_MOUSE_INTERACTOR_H

#include <domain/hydrodynamic_configuration.h>
#include "mesh_mouse_interactor.h"

#include <vtkIdTypeArray.h>
#include <vtkPolyData.h>
#include <vtkActor2D.h>

class HydrodynamicMouseInteractor : public MeshMouseInteractor {
private:
    vtkSmartPointer<vtkPolyData> meshPolyData;
    vtkSmartPointer<vtkPolyData> boundaryPolyData;
    vtkSmartPointer<vtkActor> boundaryEdgesActor;
    HydrodynamicConfiguration *hydrodynamicConfiguration;
    BoundaryCondition *currentBoundaryCondition;
    
    vtkSmartPointer<vtkPolyData> getTargetPolyData(BoundaryCondition *boundaryCondition) const;
public:
    static HydrodynamicMouseInteractor* New();
    vtkTypeMacro(HydrodynamicMouseInteractor, vtkInteractorStyleRubberBandPick);
    
    HydrodynamicMouseInteractor();
    
    void renderBoundaryCondition(BoundaryCondition *boundaryCondition);
    void removeBoundaryCondition(BoundaryCondition *boundaryCondition);
    void highlightBoundaryCondition(BoundaryCondition *boundaryCondition, bool hightlight);
    void clearSelection();
    void setHydrodynamicConfiguration(HydrodynamicConfiguration *hydrodynamicConfiguration);
    void setBoundaryCondition(BoundaryCondition *boundaryCondition);
    void pickCell();
    
    virtual void OnLeftButtonUp();
    virtual void activatePicker(const PickerMode &pickerMode);
};

#endif // HYDRODYNAMIC_MOUSE_INTERACTOR_H
