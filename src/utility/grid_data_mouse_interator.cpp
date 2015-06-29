#include "include/utility/grid_data_mouse_interactor.h"

#include <vtkExtractSelectedPolyDataIds.h>
#include <vtkInteractorObserver.h>
#include <vtkPolyDataMapper.h>
#include <vtkSelectionNode.h>
#include <vtkCellPicker.h>
#include <vtkSelection.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>

GridDataMouseInteractor::GridDataMouseInteractor() : selectedCellIds(vtkSmartPointer<vtkIdTypeArray>::New()), selectionActor(nullptr), mesh(nullptr) {}

void GridDataMouseInteractor::OnLeftButtonDown() {
    if (mesh == nullptr || selectionActor == nullptr) {
        selectedCellIds->Reset();
    } else {
        int *clickPosition = this->GetInteractor()->GetEventPosition();
        vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
        
        picker->SetTolerance(0.0005);
        picker->Pick(clickPosition[0], clickPosition[1], 0, this->GetDefaultRenderer());
        
        vtkIdType cellId = picker->GetCellId();
        
        if (cellId != -1) {
            bool inSelectionArray = false;
            
            for (vtkIdType i = 0; i < selectedCellIds->GetNumberOfTuples(); i++) {
                if (selectedCellIds->GetValue(i) == cellId) {
                    selectedCellIds->RemoveTuple(i);
                    inSelectionArray = true;
                }
            }
            
            if (!inSelectionArray) {
                selectedCellIds->InsertNextValue(cellId);
            }
            
            vtkSmartPointer<vtkSelectionNode> selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
            selectionNode->SetFieldType(vtkSelectionNode::CELL);
            selectionNode->SetContentType(vtkSelectionNode::INDICES);
            selectionNode->SetSelectionList(selectedCellIds);
            
            vtkSmartPointer<vtkSelection> selection = vtkSmartPointer<vtkSelection>::New();
            selection->AddNode(selectionNode);
            
            vtkSmartPointer<vtkExtractSelectedPolyDataIds> extractSelection = vtkSmartPointer<vtkExtractSelectedPolyDataIds>::New();
            extractSelection->SetInputData(0, mesh->getPolyData());
            extractSelection->SetInputData(1, selection);
            extractSelection->Update();
            
            vtkSmartPointer<vtkPolyData> selectionPolyData = vtkSmartPointer<vtkPolyData>::New();
            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            
            selectionPolyData->DeepCopy(extractSelection->GetOutput());
            mapper->SetInputData(selectionPolyData);
            mapper->ScalarVisibilityOff();
            selectionActor->SetMapper(mapper);
            selectionActor->GetProperty()->SetEdgeColor(1, 1, 0);
            selectionActor->GetProperty()->EdgeVisibilityOn();
            selectionActor->GetProperty()->LightingOff();
            selectionActor->GetProperty()->SetLineStipplePattern(0xF0F0);
        }
    }
    
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void GridDataMouseInteractor::setSelectionActor(vtkActor *selectionActor) {
    this->selectionActor = selectionActor;
}

void GridDataMouseInteractor::setPickMode(const PickMode &pickMode) {
    this->pickMode = pickMode;
}

void GridDataMouseInteractor::setMesh(Mesh *mesh) {
    this->mesh = mesh;
}

void GridDataMouseInteractor::clearSelection() {
    selectedCellIds->Reset();
}