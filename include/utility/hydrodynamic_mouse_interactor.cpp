#include "include/utility/hydrodynamic_mouse_interactor.h"

#include <vtkExtractSelectedPolyDataIds.h>
#include <vtkExtractSelectedFrustum.h>
#include <vtkInteractorObserver.h>
#include <vtkLabeledDataMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkSelectionNode.h>
#include <vtkTextProperty.h>
#include <vtkCellCenters.h>
#include <vtkCellPicker.h>
#include <vtkAreaPicker.h>
#include <vtkSelection.h>
#include <vtkProperty.h>
#include <vtkCellData.h>

HydrodynamicMouseInteractor::HydrodynamicMouseInteractor() :
    selectionIdLabelsActor(nullptr), selectionActor(nullptr), cellIdsArray(nullptr), meshPolyData(nullptr), cellPickMode(CellPickMode::UNDEFINED), lastCellId(-1)
{}

void HydrodynamicMouseInteractor::OnLeftButtonDown() {
    int *clickPosition = this->GetInteractor()->GetEventPosition();
    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    
    picker->SetTolerance(0.0005);
    picker->Pick(clickPosition[0], clickPosition[1], 0, this->GetDefaultRenderer());
    
    lastCellId = picker->GetCellId();
    
    vtkInteractorStyleRubberBandPick::OnLeftButtonDown();
}

void HydrodynamicMouseInteractor::OnLeftButtonUp() {
    vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
    
    if (cellPickMode == CellPickMode::MULTIPLE && meshPolyData != nullptr) {
        vtkSmartPointer<vtkExtractSelectedFrustum> extractor = vtkSmartPointer<vtkExtractSelectedFrustum>::New();
        extractor->PreserveTopologyOff();
        extractor->SetInputData(meshPolyData);
        extractor->SetFrustum(vtkAreaPicker::SafeDownCast(this->GetInteractor()->GetPicker())->GetFrustum());
        extractor->Update();
        
        vtkSmartPointer<vtkPolyData> selectionPolyData = vtkSmartPointer<vtkPolyData>::New();
        selectionPolyData->ShallowCopy(extractor->GetOutput());
        
        if (selectionPolyData->GetCellData()->HasArray("vtkOriginalCellIds")) {
            cellIdsArray->SetNumberOfTuples(selectionPolyData->GetCellData()->GetScalars("vtkOriginalCellIds")->GetNumberOfTuples());
            cellIdsArray->Resize(cellIdsArray->GetNumberOfTuples());
            
            for (vtkIdType i = 0; i < cellIdsArray->GetNumberOfTuples(); i++) {
                cellIdsArray->SetTuple1(i, selectionPolyData->GetCellData()->GetScalars("vtkOriginalCellIds")->GetTuple1(i));
            }
            
            renderSelection();
        }
    }
}

void HydrodynamicMouseInteractor::pickCell() {
    if (lastCellId != -1 && meshPolyData != nullptr && selectionActor != nullptr) {
        bool inSelectionArray = false;
        
        for (vtkIdType i = 0; i < cellIdsArray->GetNumberOfTuples(); i++) {
            if (cellIdsArray->GetValue(i) == lastCellId) {
                cellIdsArray->RemoveTuple(i);
                inSelectionArray = true;
            }
        }
        
        if (!inSelectionArray) {
            cellIdsArray->InsertNextValue(lastCellId);
        }
        
        renderSelection();
    }
}

void HydrodynamicMouseInteractor::renderSelection() {
    vtkSmartPointer<vtkSelectionNode> selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
    selectionNode->SetFieldType(vtkSelectionNode::CELL);
    selectionNode->SetContentType(vtkSelectionNode::INDICES);
    selectionNode->SetSelectionList(cellIdsArray);
    
    vtkSmartPointer<vtkSelection> selection = vtkSmartPointer<vtkSelection>::New();
    selection->AddNode(selectionNode);
    
    vtkSmartPointer<vtkExtractSelectedPolyDataIds> extractSelection = vtkSmartPointer<vtkExtractSelectedPolyDataIds>::New();
    extractSelection->SetInputData(0, meshPolyData);
    extractSelection->SetInputData(1, selection);
    extractSelection->Update();
    
    vtkSmartPointer<vtkPolyData> selectionPolyData = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    
    selectionPolyData->DeepCopy(extractSelection->GetOutput());
    selectionPolyData->GetCellData()->SetScalars(cellIdsArray);
    
    vtkSmartPointer<vtkCellCenters> cellCentersFilter = vtkSmartPointer<vtkCellCenters>::New();
    cellCentersFilter->SetInputData(selectionPolyData);
    cellCentersFilter->Update();
    
    vtkSmartPointer<vtkLabeledDataMapper> labelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
    labelMapper->SetInputConnection(cellCentersFilter->GetOutputPort());
    labelMapper->SetLabelModeToLabelFieldData();
    labelMapper->SetFieldDataName("cellIds");
    labelMapper->GetLabelTextProperty()->SetColor(0, 0, 0);
    labelMapper->GetLabelTextProperty()->ShadowOff();
    
    selectionIdLabelsActor->SetMapper(labelMapper);
    this->GetDefaultRenderer()->AddActor2D(selectionIdLabelsActor);
    
    mapper->SetInputData(selectionPolyData);
    mapper->ScalarVisibilityOff();
    selectionActor->SetMapper(mapper);
    selectionActor->GetProperty()->SetEdgeColor(1, 1, 0);
    selectionActor->GetProperty()->EdgeVisibilityOn();
    selectionActor->GetProperty()->LightingOff();
    selectionActor->GetProperty()->SetLineStipplePattern(0xF0F0);
    
    this->GetDefaultRenderer()->GetRenderWindow()->Render();
}

vtkActor* HydrodynamicMouseInteractor::getSelectionActor() {
    return selectionActor;
}

vtkActor2D* HydrodynamicMouseInteractor::getSelectionIdLabelsActor() {
    return selectionIdLabelsActor;
}

void HydrodynamicMouseInteractor::activateCellPicking(const CellPickMode &cellPickMode, vtkIdTypeArray *cellIdsArray) {
    selectionActor = vtkSmartPointer<vtkActor>::New();
    selectionIdLabelsActor = vtkSmartPointer<vtkActor2D>::New();
    this->cellIdsArray = cellIdsArray;
    this->cellPickMode = cellPickMode;
    
    this->GetDefaultRenderer()->AddActor(selectionActor);
}

void HydrodynamicMouseInteractor::deactivateCellPicking() {
    this->cellIdsArray = nullptr;
    this->cellPickMode = CellPickMode::UNDEFINED;
    this->CurrentMode = 0; // VTKISRBP_ORIENT
    this->GetDefaultRenderer()->RemoveActor(selectionActor);
    this->GetDefaultRenderer()->RemoveActor2D(selectionIdLabelsActor);
    this->GetDefaultRenderer()->GetRenderWindow()->Render();
}

void HydrodynamicMouseInteractor::setMeshPolyData(vtkPolyData *meshPolyData) {
    this->meshPolyData = meshPolyData;
}