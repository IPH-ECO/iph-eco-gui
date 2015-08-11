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
#include <QColor>

HydrodynamicMouseInteractor::HydrodynamicMouseInteractor() :
    meshPolyData(nullptr), hydrodynamicConfiguration(nullptr), currentBoundaryCondition(nullptr), cellPickMode(CellPickMode::UNDEFINED), lastCellId(-1) {}

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
            vtkIdTypeArray *selectedCellsArray = vtkIdTypeArray::SafeDownCast(selectionPolyData->GetCellData()->GetScalars("vtkOriginalCellIds"));
            
            for (BoundaryCondition *boundaryCondition : hydrodynamicConfiguration->getBoundaryConditions()) {
                if (boundaryCondition == currentBoundaryCondition) { // Excludes current boundary condition
                    continue;
                }
                
                QSet<vtkIdType> objectIds = boundaryCondition->getObjectIds();
                
                for (vtkIdType j = 0; j < selectedCellsArray->GetNumberOfTuples(); j++) {
                    if (objectIds.contains(selectedCellsArray->GetTuple1(j))) {
                        selectedCellsArray->RemoveTuple(j--);
                    }
                }
            }
            
            currentBoundaryCondition->setObjectIds(selectedCellsArray);
            
            renderBoundaryCondition(currentBoundaryCondition);
        }
    }
}

void HydrodynamicMouseInteractor::pickCell() {
    if (lastCellId != -1 && meshPolyData != nullptr) {
        currentBoundaryCondition->addObjectId(lastCellId);
        renderBoundaryCondition(currentBoundaryCondition);
    }
}

void HydrodynamicMouseInteractor::renderBoundaryCondition(BoundaryCondition *boundaryCondition) {
    vtkSmartPointer<vtkIdTypeArray> vtkCellIds = boundaryCondition->getVTKObjectIds();
    vtkSmartPointer<vtkSelectionNode> selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
    selectionNode->SetFieldType(vtkSelectionNode::CELL);
    selectionNode->SetContentType(vtkSelectionNode::INDICES);
    selectionNode->SetSelectionList(vtkCellIds);
    
    vtkSmartPointer<vtkSelection> selection = vtkSmartPointer<vtkSelection>::New();
    selection->AddNode(selectionNode);
    
    vtkSmartPointer<vtkExtractSelectedPolyDataIds> extractSelection = vtkSmartPointer<vtkExtractSelectedPolyDataIds>::New();
    extractSelection->SetInputData(0, meshPolyData);
    extractSelection->SetInputData(1, selection);
    extractSelection->Update();
    
    vtkSmartPointer<vtkPolyData> selectionPolyData = vtkSmartPointer<vtkPolyData>::New();
    selectionPolyData->DeepCopy(extractSelection->GetOutput());
    selectionPolyData->GetCellData()->SetScalars(vtkCellIds);
    
    vtkSmartPointer<vtkCellCenters> cellCentersFilter = vtkSmartPointer<vtkCellCenters>::New();
    cellCentersFilter->SetInputData(selectionPolyData);
    cellCentersFilter->Update();
    
    vtkSmartPointer<vtkLabeledDataMapper> labelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
    std::string arrayName = boundaryCondition->getVTKObjectsArrayName().toStdString();
    labelMapper->SetInputConnection(cellCentersFilter->GetOutputPort());
    labelMapper->SetLabelModeToLabelFieldData();
    labelMapper->SetFieldDataName(arrayName.c_str());
    labelMapper->GetLabelTextProperty()->SetColor(0, 0, 0);
    labelMapper->GetLabelTextProperty()->ShadowOff();
    
    vtkSmartPointer<vtkActor2D> selectionIdLabelsActor = boundaryCondition->getLabelsActor();
    selectionIdLabelsActor->SetMapper(labelMapper);
    this->GetDefaultRenderer()->AddActor2D(selectionIdLabelsActor);
    
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(selectionPolyData);
    mapper->ScalarVisibilityOff();
    
    vtkSmartPointer<vtkActor> selectionActor = boundaryCondition->getSelectionActor();
    QColor cellColor(boundaryCondition->getCellColor());
    selectionActor->SetMapper(mapper);
    selectionActor->GetProperty()->SetColor(cellColor.redF(), cellColor.greenF(), cellColor.blueF());
    selectionActor->GetProperty()->LightingOff();
    this->GetDefaultRenderer()->AddActor(selectionActor);
    
    this->GetDefaultRenderer()->GetRenderWindow()->Render();
    
    if (boundaryCondition == currentBoundaryCondition) {
        emit objectSelected();
    }
}

void HydrodynamicMouseInteractor::highlightBoundaryCondition(BoundaryCondition *boundaryCondition, bool hightlight) {
    vtkSmartPointer<vtkActor> selectionActor = boundaryCondition->getSelectionActor();
    
    selectionActor->GetProperty()->SetEdgeColor(1, 1, 0);
    selectionActor->GetProperty()->SetLineStipplePattern(0xF0F0);
    selectionActor->GetProperty()->SetEdgeVisibility(hightlight);
    this->GetDefaultRenderer()->GetRenderWindow()->Render();
}

void HydrodynamicMouseInteractor::activateCellPicker(const CellPickMode &cellPickMode) {
    this->cellPickMode = cellPickMode;
}

void HydrodynamicMouseInteractor::deactivateCellPicker() {
    this->cellPickMode = CellPickMode::UNDEFINED;
    this->CurrentMode = 0; // VTKISRBP_ORIENT
}

void HydrodynamicMouseInteractor::clearSelection() {
    if (currentBoundaryCondition != nullptr) {
        currentBoundaryCondition->clearObjectIds();
        this->GetDefaultRenderer()->RemoveActor(currentBoundaryCondition->getSelectionActor());
        this->GetDefaultRenderer()->RemoveActor2D(currentBoundaryCondition->getLabelsActor());
        this->GetDefaultRenderer()->GetRenderWindow()->Render();
    }
}

void HydrodynamicMouseInteractor::setHydrodynamicConfiguration(HydrodynamicConfiguration *hydrodynamicConfiguration) {
    this->hydrodynamicConfiguration = hydrodynamicConfiguration;
    this->meshPolyData = hydrodynamicConfiguration->getGridDataConfiguration()->getMesh()->getPolyData();
    
    this->clearSelection();
    this->deactivateCellPicker();
}

void HydrodynamicMouseInteractor::setBoundaryCondition(BoundaryCondition *boundaryCondition) {
    currentBoundaryCondition = boundaryCondition;
}
