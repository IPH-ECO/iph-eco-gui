#ifndef GRID_DATA_MOUSE_INTERACTOR_H
#define GRID_DATA_MOUSE_INTERACTOR_H

#include "include/domain/mesh.h"
#include "mesh_mouse_interactor.h"

#include <vtkIdTypeArray.h>
#include <vtkActor2D.h>

enum class CellPickMode { INDIVIDUAL = 1, MULTIPLE };

class GridDataMouseInteractor : public MeshMouseInteractor {
private:
    vtkSmartPointer<vtkActor2D> selectionIdLabelsActor;
    vtkSmartPointer<vtkActor> selectionActor;
    CellPickMode cellPickMode;
    vtkIdTypeArray *cellIdsArray;
    vtkIdType lastCellId;
    Mesh *mesh;
    
public:
    static GridDataMouseInteractor* New();
    vtkTypeMacro(GridDataMouseInteractor, vtkInteractorStyleTrackballCamera);
    
    GridDataMouseInteractor();
    
    vtkActor* getSelectionActor();
    vtkActor2D* getSelectionIdLabelsActor();
    
    virtual void OnLeftButtonDown();
    void activateCellPicking(const CellPickMode &cellPickMode, vtkIdTypeArray *cellIdsArray);
    void deactivateCellPicking();
    void setMesh(Mesh *mesh);
    void clearSelection();
    void pickCell();
};

#endif // GRID_DATA_MOUSE_INTERACTOR_H
