#include <utility/hydrodynamic_mouse_interactor.h>

#include <vtkExtractSelectedPolyDataIds.h>
#include <vtkExtractSelectedFrustum.h>
#include <vtkInteractorObserver.h>
#include <vtkLabeledDataMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkSelectionNode.h>
#include <vtkFeatureEdges.h>
#include <vtkTextProperty.h>
#include <vtkCellCenters.h>
#include <vtkCellPicker.h>
#include <vtkAreaPicker.h>
#include <vtkSelection.h>
#include <vtkProperty.h>
#include <vtkCellData.h>
#include <QColor>

HydrodynamicMouseInteractor::HydrodynamicMouseInteractor() :
    hydrodynamicConfiguration(nullptr),
    currentBoundaryCondition(nullptr)
{}

void HydrodynamicMouseInteractor::OnLeftButtonUp() {
    vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
    
    if (pickerMode == PickerMode::MULTIPLE_CELL || pickerMode == PickerMode::MULTIPLE_EDGE) {
        vtkSmartPointer<vtkExtractSelectedFrustum> extractor = vtkSmartPointer<vtkExtractSelectedFrustum>::New();
        extractor->PreserveTopologyOff();
        extractor->SetInputData(getTargetPolyData(currentBoundaryCondition));
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

bool HydrodynamicMouseInteractor::pickCell() {
    if (pickerMode == PickerMode::EACH_CELL && lastCellId != -1) {
        currentBoundaryCondition->addObjectId(lastCellId);
        return renderBoundaryCondition(currentBoundaryCondition);
    }
    
    return false;
}

bool HydrodynamicMouseInteractor::renderBoundaryCondition(BoundaryCondition *boundaryCondition) {
    vtkSmartPointer<vtkIdTypeArray> objectIds = boundaryCondition->getVTKObjectIds();
    vtkSmartPointer<vtkSelectionNode> selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
    selectionNode->SetFieldType(vtkSelectionNode::CELL);
    selectionNode->SetContentType(vtkSelectionNode::INDICES);
    selectionNode->SetSelectionList(objectIds);
    
    vtkSmartPointer<vtkSelection> selection = vtkSmartPointer<vtkSelection>::New();
    selection->AddNode(selectionNode);
    
    vtkSmartPointer<vtkExtractSelectedPolyDataIds> extractSelection = vtkSmartPointer<vtkExtractSelectedPolyDataIds>::New();
    vtkSmartPointer<vtkPolyData> targetPolyData = getTargetPolyData(boundaryCondition);
    extractSelection->SetInputData(0, targetPolyData);
    extractSelection->SetInputData(1, selection);
    extractSelection->Update();
    
    vtkSmartPointer<vtkPolyData> selectionPolyData = vtkSmartPointer<vtkPolyData>::New();
    selectionPolyData->DeepCopy(extractSelection->GetOutput());
    selectionPolyData->GetCellData()->SetScalars(objectIds);
    
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
    selectionActor->GetProperty()->SetLineWidth(pickerMode == PickerMode::MULTIPLE_EDGE || boundaryCondition->getType() == BoundaryConditionType::WATER_FLOW ? 2 : 1);
    this->GetDefaultRenderer()->AddActor(selectionActor);
    
    this->GetDefaultRenderer()->GetRenderWindow()->Render();
    
    return pickerMode != PickerMode::NO_PICKER;
}

void HydrodynamicMouseInteractor::removeBoundaryCondition(BoundaryCondition *boundaryCondition) {
    this->highlightBoundaryCondition(boundaryCondition, false);
    this->GetDefaultRenderer()->RemoveActor(boundaryCondition->getSelectionActor());
    this->GetDefaultRenderer()->RemoveActor(boundaryCondition->getLabelsActor());
    this->GetDefaultRenderer()->GetRenderWindow()->Render();
}

void HydrodynamicMouseInteractor::highlightBoundaryCondition(BoundaryCondition *boundaryCondition, bool highlight) {
    vtkSmartPointer<vtkActor> selectionActor = boundaryCondition->getSelectionActor();
    
    if (selectionActor) {
        selectionActor->GetProperty()->SetLineStipplePattern(highlight ? 0xF0F0 : 0xFFFF);
        selectionActor->GetProperty()->SetEdgeVisibility(highlight);
        selectionActor->GetProperty()->SetEdgeColor(1, 1, 0);
        this->GetDefaultRenderer()->GetRenderWindow()->Render();
    }
}

void HydrodynamicMouseInteractor::activatePicker(const PickerMode &pickerMode) {
    if (pickerMode == PickerMode::MULTIPLE_EDGE) {
        vtkSmartPointer<vtkPolyDataMapper> boundaryEdgesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        boundaryEdgesMapper->SetInputData(boundaryPolyData);
        boundaryEdgesMapper->ScalarVisibilityOff();
        
        if (!boundaryEdgesActor) {
            boundaryEdgesActor = vtkSmartPointer<vtkActor>::New();
        }
        boundaryEdgesActor->SetMapper(boundaryEdgesMapper);
        boundaryEdgesActor->GetProperty()->SetColor(0, 0, 0);
        
        this->GetDefaultRenderer()->AddActor(boundaryEdgesActor);
        this->GetDefaultRenderer()->GetRenderWindow()->Render();
    }

    MeshMouseInteractor::activatePicker(pickerMode);
}

void HydrodynamicMouseInteractor::clearSelection() {
    if (currentBoundaryCondition) {
        currentBoundaryCondition->clearObjectIds();
        this->GetDefaultRenderer()->RemoveActor(currentBoundaryCondition->getSelectionActor());
        this->GetDefaultRenderer()->RemoveActor2D(currentBoundaryCondition->getLabelsActor());
        this->GetDefaultRenderer()->GetRenderWindow()->Render();
    }
}

void HydrodynamicMouseInteractor::setHydrodynamicConfiguration(HydrodynamicConfiguration *hydrodynamicConfiguration) {
    if (hydrodynamicConfiguration == nullptr) {
        if (this->hydrodynamicConfiguration != nullptr) {
            for (BoundaryCondition *boundaryCondition : this->hydrodynamicConfiguration->getBoundaryConditions()) {
                removeBoundaryCondition(boundaryCondition);
            }
            this->GetDefaultRenderer()->RemoveActor(this->boundaryEdgesActor);
        }
    } else {
        this->hydrodynamicConfiguration = hydrodynamicConfiguration;
        this->meshPolyData = hydrodynamicConfiguration->getGridDataConfiguration()->getMesh()->getMeshPolyData();
        this->boundaryPolyData = hydrodynamicConfiguration->getGridDataConfiguration()->getMesh()->getBoundaryPolyData();
        
        this->clearSelection();
        MeshMouseInteractor::deactivatePicker();
    }
}

void HydrodynamicMouseInteractor::setBoundaryCondition(BoundaryCondition *boundaryCondition) {
    currentBoundaryCondition = boundaryCondition;
}

vtkSmartPointer<vtkPolyData> HydrodynamicMouseInteractor::getTargetPolyData(BoundaryCondition *boundaryCondition) const {
    if (pickerMode == PickerMode::NO_PICKER) {
        return boundaryCondition->getType() == BoundaryConditionType::WATER_LEVEL ? meshPolyData : boundaryPolyData;
    }
    
    return pickerMode == PickerMode::MULTIPLE_EDGE ? boundaryPolyData : meshPolyData;
}