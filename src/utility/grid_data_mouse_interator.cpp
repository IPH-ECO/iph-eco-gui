#include "include/utility/grid_data_mouse_interactor.h"

#include <vtkExtractSelectedPolyDataIds.h>
#include <vtkInteractorObserver.h>
#include <vtkLabeledDataMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkSelectionNode.h>
#include <vtkCellCenters.h>
#include <vtkCellPicker.h>
#include <vtkSelection.h>
#include <vtkFieldData.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkCellData.h>

GridDataMouseInteractor::GridDataMouseInteractor() : selectedCellIds(nullptr), selectionIdLabelsActor(nullptr), selectionActor(nullptr), mesh(nullptr) {}

void GridDataMouseInteractor::OnLeftButtonDown() {
    if (mesh != nullptr && selectionActor != nullptr) {
        int *clickPosition = this->GetInteractor()->GetEventPosition();
        vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
        
        picker->SetTolerance(0.0005);
        picker->Pick(clickPosition[0], clickPosition[1], 0, DefaultRenderer);
        
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
            selectionPolyData->GetCellData()->SetScalars(selectedCellIds);
            
            vtkSmartPointer<vtkCellCenters> cellCentersFilter = vtkSmartPointer<vtkCellCenters>::New();
            cellCentersFilter->SetInputData(selectionPolyData);
            cellCentersFilter->Update();
            
            vtkSmartPointer<vtkLabeledDataMapper> labelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
            labelMapper->SetInputConnection(cellCentersFilter->GetOutputPort());
            labelMapper->SetLabelModeToLabelFieldData();
            
            selectionIdLabelsActor->SetMapper(labelMapper);
            
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

vtkActor* GridDataMouseInteractor::getSelectionActor() {
    return selectionActor;
}

vtkActor2D* GridDataMouseInteractor::getSelectionIdLabelsActor() {
    return selectionIdLabelsActor;
}

void GridDataMouseInteractor::activateCellPicking(const CellPickMode &cellPickMode) {
    selectionActor = vtkSmartPointer<vtkActor>::New();
    selectionIdLabelsActor = vtkSmartPointer<vtkActor2D>::New();
    selectedCellIds = vtkSmartPointer<vtkIdTypeArray>::New();
    
    DefaultRenderer->AddActor(selectionActor);
    DefaultRenderer->AddActor2D(selectionIdLabelsActor);
}

void GridDataMouseInteractor::deactivateCellPicking() {
    DefaultRenderer->RemoveActor(selectionActor);
    DefaultRenderer->RemoveActor2D(selectionIdLabelsActor);
    DefaultRenderer->GetRenderWindow()->Render();
}

void GridDataMouseInteractor::setMesh(Mesh *mesh) {
    this->mesh = mesh;
}