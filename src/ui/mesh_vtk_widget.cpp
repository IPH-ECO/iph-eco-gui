#include <ui/mesh_vtk_widget.h>

#include <QList>
#include <vtkVertex.h>
#include <vtkCamera.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkCellArray.h>
#include <vtkPNGWriter.h>
#include <vtkAreaPicker.h>
#include <vtkCellCenters.h>
#include <vtkExtractEdges.h>
#include <vtkTextProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkLabeledDataMapper.h>
#include <vtkWindowToImageFilter.h>
#include <vtkInteractorStyleRubberBandZoom.h>

vtkStandardNewMacro(MeshMouseInteractor);

MeshVTKWidget::MeshVTKWidget(QWidget *parent, MeshMouseInteractor *mouseInteractor) :
    QVTKWidget(parent),
    renderer(vtkSmartPointer<vtkRenderer>::New()),
    currentMesh(nullptr),
    showBoundaryEdges(true),
    showMesh(true),
    showAxes(true)
{
    vtkSmartPointer<vtkAreaPicker> areaPicker = vtkSmartPointer<vtkAreaPicker>::New();
    
    if (mouseInteractor) {
        this->mouseInteractor = mouseInteractor;
    } else {
        this->mouseInteractor = vtkSmartPointer<MeshMouseInteractor>::New();
    }
    
    renderer->SetBackground(1, 1, 1);
    this->mouseInteractor->SetDefaultRenderer(renderer);
    this->GetRenderWindow()->AddRenderer(renderer);
    this->GetInteractor()->SetInteractorStyle(this->mouseInteractor);
    this->GetInteractor()->SetPicker(areaPicker);
}

MeshVTKWidget::~MeshVTKWidget() {}

void MeshVTKWidget::render(Mesh *mesh) {
    clear();

    if (!mesh) {
        return;
    }
    
    MeshPolygon *boundaryPolygon = mesh->getBoundaryPolygon();

    if (!boundaryPolygon || !boundaryPolygon->getFilteredPolygon()) {
        return;
    }
    
    currentMesh = mesh;
    
    // Mesh rendering
    vtkSmartPointer<vtkExtractEdges> meshEdges = vtkSmartPointer<vtkExtractEdges>::New();
    meshEdges->SetInputData(mesh->getMeshPolyData());
    meshEdges->Update();
    
    vtkSmartPointer<vtkPolyDataMapper> meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    meshMapper->SetInputConnection(meshEdges->GetOutputPort());
    meshMapper->ScalarVisibilityOff();
    
    QColor meshColor(mesh->getColor());
    
    meshActor = vtkSmartPointer<vtkActor>::New();
    meshActor->SetMapper(meshMapper);
    meshActor->GetProperty()->EdgeVisibilityOn();
    meshActor->SetVisibility(showMesh);
    this->changeMeshProperties(mesh);
    
    renderer->AddActor(meshActor);
    
    renderAxesActor();

    renderer->ResetCamera();
    renderer->GetRenderWindow()->Render();
}

void MeshVTKWidget::renderAxesActor() {
    renderer->RemoveActor(axesActor);
    
    axesActor = vtkSmartPointer<vtkCubeAxesActor>::New();
    axesActor->SetXUnits("m");
    axesActor->SetXLabelFormat("%4.2f");
    axesActor->SetYUnits("m");
    axesActor->SetYLabelFormat("%4.2f");
    axesActor->SetZUnits("m");
    axesActor->SetZLabelFormat("%0.2f");
    axesActor->GetLabelTextProperty(0)->SetColor(0, 0, 0);
    axesActor->GetTitleTextProperty(0)->SetColor(0, 0, 0);
    axesActor->GetXAxesLinesProperty()->SetColor(0, 0, 0);
    axesActor->GetLabelTextProperty(1)->SetColor(0, 0, 0);
    axesActor->GetTitleTextProperty(1)->SetColor(0, 0, 0);
    axesActor->GetYAxesLinesProperty()->SetColor(0, 0, 0);
    axesActor->GetLabelTextProperty(2)->SetColor(0, 0, 0);
    axesActor->GetTitleTextProperty(2)->SetColor(0, 0, 0);
    axesActor->GetZAxesLinesProperty()->SetColor(0, 0, 0);
    axesActor->SetBounds(meshActor->GetBounds());
    axesActor->SetCamera(renderer->GetActiveCamera());
    axesActor->SetFlyModeToStaticEdges();
    axesActor->SetVisibility(showAxes);
    
    renderer->AddActor(axesActor);
}

void MeshVTKWidget::clear() {
    currentMesh = nullptr;
    renderer->RemoveActor(meshActor);
    renderer->RemoveActor(axesActor);
    this->GetRenderWindow()->Render();
}

void MeshVTKWidget::toggleMesh(bool show) {
    this->showMesh = show;
    if (this->meshActor) {
        this->meshActor->SetVisibility(show);
        this->GetRenderWindow()->Render();
    }
}

void MeshVTKWidget::toggleAxes(bool show) {
    this->showAxes = show;
    if (this->axesActor) {
        this->axesActor->SetVisibility(show);
        this->GetRenderWindow()->Render();
    }
}

void MeshVTKWidget::resetZoom() {
    renderer->GetActiveCamera()->SetPosition(0, 0, 0);
    renderer->GetActiveCamera()->SetFocalPoint(0, 0, -1);
    renderer->GetActiveCamera()->SetViewUp(0, 1, 0);
    renderer->ResetCamera();
    this->GetRenderWindow()->Render();
}

void MeshVTKWidget::toggleZoomArea(bool activate) {
    if (activate) {
        vtkSmartPointer<vtkInteractorStyleRubberBandZoom> zoomAreaInteractor = vtkSmartPointer<vtkInteractorStyleRubberBandZoom>::New();
        this->GetInteractor()->SetInteractorStyle(zoomAreaInteractor);
    } else {
        this->GetInteractor()->SetInteractorStyle(mouseInteractor);
    }
}

void MeshVTKWidget::toggleLabels(const LabelType &labelType) {
    renderer->RemoveActor2D(labelsActor);
    renderer->RemoveActor(verticesActor);
    
    if (labelType != LabelType::UNDEFINED) {
        vtkSmartPointer<vtkLabeledDataMapper> labelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
        labelsActor = vtkSmartPointer<vtkActor2D>::New();
        
        if (labelType == LabelType::CELL_ID) {
            vtkSmartPointer<vtkCellCenters> cellCentersFilter = vtkSmartPointer<vtkCellCenters>::New();
            cellCentersFilter->SetInputData(currentMesh->getMeshPolyData());
            cellCentersFilter->Update();
            
            labelMapper->SetInputConnection(cellCentersFilter->GetOutputPort());
        } else if (labelType == LabelType::VERTICE_ID) {
            vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
            vtkSmartPointer<vtkPolyData> meshPolyData = currentMesh->getMeshPolyData();
            vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
            
            points->SetNumberOfPoints(meshPolyData->GetNumberOfPoints());
            
            for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++) {
                vtkSmartPointer<vtkVertex> vertex = vtkSmartPointer<vtkVertex>::New();
                double point[3];
                
                meshPolyData->GetPoints()->GetPoint(i, point);
                points->SetPoint(i, point);
                vertex->GetPointIds()->SetId(0, i);
                vertices->InsertNextCell(vertex);
            }
            
            vtkSmartPointer<vtkPolyData> verticesPolyData = vtkSmartPointer<vtkPolyData>::New();
            verticesPolyData->SetPoints(points);
            verticesPolyData->SetVerts(vertices);
            
            vtkSmartPointer<vtkPolyDataMapper> verticesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            verticesMapper->SetInputData(verticesPolyData);
            
            verticesActor = vtkSmartPointer<vtkActor>::New();
            verticesActor->SetMapper(verticesMapper);
            verticesActor->GetProperty()->SetPointSize(2);
            verticesActor->GetProperty()->SetColor(1, 0, 0);
            
            renderer->AddActor(verticesActor);
            labelMapper->SetInputData(currentMesh->getMeshPolyData());
        }
        
        labelMapper->SetLabelModeToLabelIds();
        labelMapper->GetLabelTextProperty()->SetColor(0, 0, 0);
        labelMapper->GetLabelTextProperty()->BoldOff();
        labelMapper->GetLabelTextProperty()->ShadowOff();
            
        labelsActor->SetMapper(labelMapper);
        
        renderer->AddActor2D(labelsActor);
    }
    
    this->GetRenderWindow()->Render();
}

void MeshVTKWidget::changeBackgroundColor(const double &r, const double &g, const double &b) {
    renderer->SetBackground(r, g, b);
    this->GetRenderWindow()->Render();
}

void MeshVTKWidget::exportToImage(const QString &filename) {
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(this->GetRenderWindow());
    windowToImageFilter->SetMagnification(1); //set the resolution of the output image (3 times the current resolution of vtk render window)
    windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
    windowToImageFilter->ReadFrontBufferOff(); // read from the back buffer
    windowToImageFilter->Update();
    
    QByteArray cFilename = filename.toLocal8Bit();
    vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetFileName(cFilename.constData());
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    writer->Write();
}

Mesh* MeshVTKWidget::getMesh() const {
    return currentMesh;
}

void MeshVTKWidget::changeMeshProperties(Mesh *mesh) {
    if (meshActor) {
        QColor meshColor(mesh->getColor());
        
        meshActor->GetProperty()->SetEdgeColor(meshColor.redF(), meshColor.greenF(), meshColor.blueF());
        meshActor->GetProperty()->SetLineStipplePattern(mesh->getLineStyle());
        meshActor->GetProperty()->SetLineWidth(mesh->getLineWidth());
        meshActor->GetProperty()->SetOpacity(mesh->getOpacity() / 100.0);
        
        this->GetRenderWindow()->Render();
    }
}