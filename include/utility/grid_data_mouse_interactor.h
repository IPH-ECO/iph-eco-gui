#ifndef GRID_DATA_MOUSE_INTERACTOR_H
#define GRID_DATA_MOUSE_INTERACTOR_H

#include "include/domain/mesh.h"
#include "mesh_mouse_interactor.h"

#include <vtkIdTypeArray.h>

enum class PickMode { INDIVIDUAL = 1, MULTIPLE };

class GridDataMouseInteractor : public MeshMouseInteractor {
private:
    vtkSmartPointer<vtkIdTypeArray> selectedCellIds;
    vtkActor *selectionActor;
    PickMode pickMode;
    Mesh *mesh;
    
public:
    static GridDataMouseInteractor* New();
    vtkTypeMacro(GridDataMouseInteractor, vtkInteractorStyleTrackballCamera);
    
    GridDataMouseInteractor();
    
    virtual void OnLeftButtonDown();
    void setSelectionActor(vtkActor *selectionActor);
    void setPickMode(const PickMode &pickMode);
    void setMesh(Mesh *mesh);
    void clearSelection();
};

#endif // GRID_DATA_MOUSE_INTERACTOR_H
