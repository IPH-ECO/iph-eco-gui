#include "include/utility/grid_data_mouse_interactor.h"

#include <vtkExtractSelectedPolyDataIds.h>
#include <vtkInteractorObserver.h>
#include <vtkLabeledDataMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkSelectionNode.h>
#include <vtkTextProperty.h>
#include <vtkCellCenters.h>
#include <vtkCellPicker.h>
#include <vtkSelection.h>
#include <vtkFieldData.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkCellData.h>

GridDataMouseInteractor::GridDataMouseInteractor() : selectionIdLabelsActor(nullptr), selectionActor(nullptr), cellIdsArray(nullptr), lastCellId(-1), mesh(nullptr) {}

void GridDataMouseInteractor::OnLeftButtonDown() {
    int *clickPosition = this->GetInteractor()->GetEventPosition();
    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New(); // make a attribute
    
    picker->SetTolerance(0.0005);
    picker->Pick(clickPosition[0], clickPosition[1], 0, DefaultRenderer);
    
    lastCellId = picker->GetCellId();
    
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void GridDataMouseInteractor::pickCell() {
    if (lastCellId != -1 && mesh != nullptr && selectionActor != nullptr) {
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
        
        vtkSmartPointer<vtkSelectionNode> selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
        selectionNode->SetFieldType(vtkSelectionNode::CELL);
        selectionNode->SetContentType(vtkSelectionNode::INDICES);
        selectionNode->SetSelectionList(cellIdsArray);
        
        vtkSmartPointer<vtkSelection> selection = vtkSmartPointer<vtkSelection>::New();
        selection->AddNode(selectionNode);
        
        vtkSmartPointer<vtkExtractSelectedPolyDataIds> extractSelection = vtkSmartPointer<vtkExtractSelectedPolyDataIds>::New();
        extractSelection->SetInputData(0, mesh->getPolyData());
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
        DefaultRenderer->AddActor2D(selectionIdLabelsActor);
        
        mapper->SetInputData(selectionPolyData);
        mapper->ScalarVisibilityOff();
        selectionActor->SetMapper(mapper);
        selectionActor->GetProperty()->SetEdgeColor(1, 1, 0);
        selectionActor->GetProperty()->EdgeVisibilityOn();
        selectionActor->GetProperty()->LightingOff();
        selectionActor->GetProperty()->SetLineStipplePattern(0xF0F0);
    }
}

vtkActor* GridDataMouseInteractor::getSelectionActor() {
    return selectionActor;
}

vtkActor2D* GridDataMouseInteractor::getSelectionIdLabelsActor() {
    return selectionIdLabelsActor;
}

void GridDataMouseInteractor::activateCellPicking(const CellPickMode &cellPickMode, vtkIdTypeArray *cellIdsArray) {
    selectionActor = vtkSmartPointer<vtkActor>::New();
    selectionIdLabelsActor = vtkSmartPointer<vtkActor2D>::New();
    this->cellIdsArray = cellIdsArray;
    
    DefaultRenderer->AddActor(selectionActor);
}

void GridDataMouseInteractor::deactivateCellPicking() {
    this->cellIdsArray = nullptr;
    DefaultRenderer->RemoveActor(selectionActor);
    DefaultRenderer->RemoveActor2D(selectionIdLabelsActor);
    DefaultRenderer->GetRenderWindow()->Render();
}

void GridDataMouseInteractor::setMesh(Mesh *mesh) {
    this->mesh = mesh;
}