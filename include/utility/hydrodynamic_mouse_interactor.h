#ifndef HYDRODYNAMIC_MOUSE_INTERACTOR_H
#define HYDRODYNAMIC_MOUSE_INTERACTOR_H

#include "include/domain/hydrodynamic_configuration.h"
#include "include/application/iph_types.h"
#include "mesh_mouse_interactor.h"

#include <vtkIdTypeArray.h>
#include <vtkPolyData.h>
#include <vtkActor2D.h>

class HydrodynamicMouseInteractor : public MeshMouseInteractor {
    Q_OBJECT
private:
    vtkPolyData *meshPolyData;
    HydrodynamicConfiguration *hydrodynamicConfiguration;
    BoundaryCondition *currentBoundaryCondition;
    PickerMode pickerMode;
    vtkIdType lastCellId;
public:
    static HydrodynamicMouseInteractor* New();
    vtkTypeMacro(HydrodynamicMouseInteractor, vtkInteractorStyleRubberBandPick);
    
    HydrodynamicMouseInteractor();
    
    void renderBoundaryCondition(BoundaryCondition *boundaryCondition);
    void removeBoundaryCondition(BoundaryCondition *boundaryCondition);
    void highlightBoundaryCondition(BoundaryCondition *boundaryCondition, bool hightlight);
    void activateCellPicker(const PickerMode &pickerMode);
    void deactivateCellPicker();
    void clearSelection();
    void setHydrodynamicConfiguration(HydrodynamicConfiguration *hydrodynamicConfiguration);
    void setBoundaryCondition(BoundaryCondition *boundaryCondition);
    void pickCell();
    
    virtual void OnLeftButtonDown();
    virtual void OnLeftButtonUp();
signals:
    void objectSelected();
};

#endif // HYDRODYNAMIC_MOUSE_INTERACTOR_H
