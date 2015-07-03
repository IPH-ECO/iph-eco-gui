#ifndef GRID_DATA_MOUSE_INTERACTOR_H
#define GRID_DATA_MOUSE_INTERACTOR_H

#include "mesh_mouse_interactor.h"

#include <vtkIdTypeArray.h>
#include <vtkPolyData.h>
#include <vtkActor2D.h>

enum class CellPickMode { INDIVIDUAL = 1, MULTIPLE, UNDEFINED };

class GridDataMouseInteractor : public MeshMouseInteractor {
private:
    vtkSmartPointer<vtkActor2D> selectionIdLabelsActor;
    vtkSmartPointer<vtkActor> selectionActor;
    vtkIdTypeArray *cellIdsArray;
    vtkPolyData *meshPolyData;
    CellPickMode cellPickMode;
    vtkIdType lastCellId;
    
    void renderSelection();
public:
    static GridDataMouseInteractor* New();
    vtkTypeMacro(GridDataMouseInteractor, vtkInteractorStyleRubberBandPick);
    
    GridDataMouseInteractor();
    
    vtkActor* getSelectionActor();
    vtkActor2D* getSelectionIdLabelsActor();
    
    virtual void OnLeftButtonDown();
    virtual void OnLeftButtonUp();
    
    void activateCellPicking(const CellPickMode &cellPickMode, vtkIdTypeArray *cellIdsArray);
    void deactivateCellPicking();
    void setMeshPolyData(vtkPolyData *meshPolyData);
    void clearSelection();
    void pickCell();
};

#endif // GRID_DATA_MOUSE_INTERACTOR_H
