#ifndef GRID_DATA_MOUSE_INTERACTOR_H
#define GRID_DATA_MOUSE_INTERACTOR_H

#include "mesh_mouse_interactor.h"

#include <vtkIdTypeArray.h>
#include <vtkPolyData.h>
#include <vtkActor2D.h>

class GridDataMouseInteractor : public MeshMouseInteractor {
private:
    vtkSmartPointer<vtkActor2D> selectionIdLabelsActor;
    vtkSmartPointer<vtkActor> selectionActor;
    vtkIdTypeArray *cellIdsArray;
    vtkPolyData *meshPolyData;
    
    void renderSelection();
public:
    static GridDataMouseInteractor* New();
    vtkTypeMacro(GridDataMouseInteractor, vtkInteractorStyleRubberBandPick);
    
    GridDataMouseInteractor();
    
    vtkActor* getSelectionActor();
    vtkActor2D* getSelectionIdLabelsActor();
    
    virtual void OnLeftButtonUp();
    virtual void deactivatePicker();

    void activatePickerWithCellIds(const PickerMode &pickerMode, vtkIdTypeArray *cellIdsArray);
    void setMeshPolyData(vtkPolyData *meshPolyData);
    void clearSelection();
    void pickCell();
};

#endif // GRID_DATA_MOUSE_INTERACTOR_H
