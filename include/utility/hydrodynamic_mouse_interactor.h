#ifndef HYDRODYNAMIC_MOUSE_INTERACTOR_H
#define HYDRODYNAMIC_MOUSE_INTERACTOR_H

#include "include/application/iph_types.h"
#include "mesh_mouse_interactor.h"

#include <vtkIdTypeArray.h>
#include <vtkPolyData.h>
#include <vtkActor2D.h>

class HydrodynamicMouseInteractor : public MeshMouseInteractor {
    Q_OBJECT
private:
    vtkSmartPointer<vtkActor2D> selectionIdLabelsActor;
    vtkSmartPointer<vtkActor> selectionActor;
    vtkIdTypeArray *cellIdsArray;
    vtkPolyData *meshPolyData;
    CellPickMode cellPickMode;
    vtkIdType lastCellId;
    
    void renderSelection();
public:
    static HydrodynamicMouseInteractor* New();
    vtkTypeMacro(HydrodynamicMouseInteractor, vtkInteractorStyleRubberBandPick);
    
    HydrodynamicMouseInteractor();
    
    vtkActor* getSelectionActor();
    vtkActor2D* getSelectionIdLabelsActor();
    
    virtual void OnLeftButtonDown();
    virtual void OnLeftButtonUp();
    
    void activateCellPicker(const CellPickMode &cellPickMode, vtkIdTypeArray *cellIdsArray);
    void deactivateCellPicker();
    void clearSelection();
    void setMeshPolyData(vtkPolyData *meshPolyData);
    void pickCell();
signals:
    void objectSelected();
};

#endif // HYDRODYNAMIC_MOUSE_INTERACTOR_H
