#include "include/ui/mesh_vtk_widget.h"

#include <vtkVertex.h>
#include <vtkPoints.h>
#include <vtkPolygon.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkCellArray.h>
#include <vtkPNGWriter.h>
#include <vtkProperty2D.h>
#include <vtkAreaPicker.h>
#include <vtkCellCenters.h>
#include <vtkSmartPointer.h>
#include <vtkExtractEdges.h>
#include <vtkRenderWindow.h>
#include <vtkWorldPointPicker.h>
#include <vtkLabeledDataMapper.h>
#include <vtkWindowToImageFilter.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleRubberBandZoom.h>
#include <QList>

#include "include/ui/structured_mesh_dialog.h"
#include "include/ui/unstructured_mesh_dialog.h"

vtkStandardNewMacro(MeshMouseInteractor);

MeshVTKWidget::MeshVTKWidget(QWidget *parent) : QVTKWidget(parent), showBoundaryEdges(true), showMesh(true), showAxes(true) {
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    mouseInteractor = vtkSmartPointer<MeshMouseInteractor>::New();
    vtkSmartPointer<vtkAreaPicker> areaPicker = vtkSmartPointer<vtkAreaPicker>::New();
    
    renderer->SetBackground(1, 1, 1);
    renderWindow->AddRenderer(renderer);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(mouseInteractor);
    renderWindowInteractor->SetPicker(areaPicker);
    mouseInteractor->SetDefaultRenderer(renderer);
    
    this->SetRenderWindow(renderWindow);
    renderWindow->Render();
}

MeshVTKWidget::~MeshVTKWidget() {}

void MeshVTKWidget::render(Mesh *mesh) {
    if (mesh == nullptr || currentMesh == mesh) {
        return;
    }
    
    currentMesh = mesh;
    
    MeshPolygon *boundaryPolygon = mesh->getBoundaryPolygon();

    if (!boundaryPolygon->getFilteredPolygon()) {
        return;
    }
    
    clear();

    // Contour rendering
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();
    QList<MeshPolygon*> meshPolygons = mesh->getIslands();
    int count = 0;

    meshPolygons.prepend(boundaryPolygon);

    for (MeshPolygon* meshPolygon : meshPolygons) {
        vtkPolygon *vtkMeshPolygon = meshPolygon->getFilteredPolygon();

        for (vtkIdType i = 0; i < vtkMeshPolygon->GetPoints()->GetNumberOfPoints(); i++) {
            double point[3];

            vtkMeshPolygon->GetPoints()->GetPoint(i, point); // Safe call
            vtkMeshPolygon->GetPointIds()->SetId(i, count);
            points->InsertNextPoint(point);
            count++;
        }

        polygons->InsertNextCell(vtkMeshPolygon);
    }
    
    vtkSmartPointer<vtkPolyData> boundaryPolyData = vtkSmartPointer<vtkPolyData>::New();
    boundaryPolyData->SetPoints(points);
    boundaryPolyData->SetPolys(polygons);

    vtkSmartPointer<vtkExtractEdges> boundaryEdges = vtkSmartPointer<vtkExtractEdges>::New();
    boundaryEdges->SetInputData(boundaryPolyData);
    boundaryEdges->Update();

    vtkSmartPointer<vtkPolyDataMapper> boundaryEdgesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    boundaryEdgesMapper->SetInputData(boundaryEdges->GetOutput());
    
    boundaryEdgesActor = vtkSmartPointer<vtkActor>::New();
    boundaryEdgesActor->SetMapper(boundaryEdgesMapper);
    boundaryEdgesActor->GetProperty()->EdgeVisibilityOn();
    boundaryEdgesActor->SetVisibility(showBoundaryEdges);
    
    // Mesh rendering
    vtkSmartPointer<vtkExtractEdges> meshEdges = vtkSmartPointer<vtkExtractEdges>::New();
    meshEdges->SetInputData(mesh->getMeshPolyData());
    meshEdges->Update();
    
    vtkSmartPointer<vtkPolyDataMapper> meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    meshMapper->SetInputConnection(meshEdges->GetOutputPort());
    meshMapper->ScalarVisibilityOff();
    
    meshActor = vtkSmartPointer<vtkActor>::New();
    meshActor->SetMapper(meshMapper);
    meshActor->GetProperty()->EdgeVisibilityOn();
    meshActor->SetVisibility(showMesh);
    
    axesActor = vtkSmartPointer<vtkCubeAxesActor>::New();
    axesActor->SetXUnits("m");
    axesActor->SetXLabelFormat("%4.2f");
    axesActor->SetYUnits("m");
    axesActor->SetYLabelFormat("%4.2f");
    axesActor->ZAxisVisibilityOff();
    axesActor->GetLabelTextProperty(0)->SetColor(0, 0, 0);
    axesActor->GetTitleTextProperty(0)->SetColor(0, 0, 0);
    axesActor->GetXAxesLinesProperty()->SetColor(0, 0, 0);
    axesActor->GetLabelTextProperty(1)->SetColor(0, 0, 0);
    axesActor->GetTitleTextProperty(1)->SetColor(0, 0, 0);
    axesActor->GetYAxesLinesProperty()->SetColor(0, 0, 0);
    axesActor->SetBounds(mesh->getMeshPolyData()->GetBounds());
    axesActor->SetCamera(renderer->GetActiveCamera());
    axesActor->SetFlyModeToStaticEdges();
    axesActor->SetVisibility(showAxes);

    if (mesh->instanceOf("StructuredMesh")) {
        StructuredMeshDialog *structuredMeshDialog = (StructuredMeshDialog*) this->parent();
        QObject::connect(mouseInteractor, SIGNAL(coordinateChanged(double&, double&)), structuredMeshDialog, SLOT(setCoordinate(double&, double&)));
        structuredMeshDialog->setArea(mesh->area());
    } else {
        UnstructuredMeshDialog *unstructuredMeshDialog = (UnstructuredMeshDialog*) this->parent();
        QObject::connect(mouseInteractor, SIGNAL(coordinateChanged(double&, double&)), unstructuredMeshDialog, SLOT(setCoordinate(double&, double&)));
        unstructuredMeshDialog->setArea(mesh->area());
    }

    renderer->AddActor(boundaryEdgesActor);
    renderer->AddActor(meshActor);
    renderer->AddActor(axesActor);
    renderer->ResetCamera();
}

void MeshVTKWidget::clear() {
    renderer->RemoveActor(axesActor);
    renderer->RemoveActor(meshActor);
    renderer->RemoveActor(boundaryEdgesActor);
    this->update();
}

void MeshVTKWidget::toggleBoundaryEdges(bool show) {
    this->showBoundaryEdges = show;
    if (this->boundaryEdgesActor) {
        this->boundaryEdgesActor->SetVisibility(show);
        this->update();
    }
}

void MeshVTKWidget::toggleMesh(bool show) {
    this->showMesh = show;
    if (this->meshActor) {
        this->meshActor->SetVisibility(show);
        this->update();
    }
}

void MeshVTKWidget::toggleAxes(bool show) {
    this->showAxes = show;
    this->axesActor->SetVisibility(show);
    this->update();
}

void MeshVTKWidget::resetZoom() {
    renderer->ResetCamera();
    this->update();
}

void MeshVTKWidget::toggleZoomArea(bool activate) {
    if (activate) {
        vtkSmartPointer<vtkInteractorStyleRubberBandZoom> zoomAreaInteractor = vtkSmartPointer<vtkInteractorStyleRubberBandZoom>::New();
        renderWindowInteractor->SetInteractorStyle(zoomAreaInteractor);
    } else {
        renderWindowInteractor->SetInteractorStyle(mouseInteractor);
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
    
    renderWindow->Render();
}

void MeshVTKWidget::changeBackgroundColor(const double &r, const double &g, const double &b) {
    renderer->SetBackground(r, g, b);
    this->update();
}

void MeshVTKWidget::exportToImage(const QString &fileName) {
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(renderWindow);
    windowToImageFilter->SetMagnification(1); //set the resolution of the output image (3 times the current resolution of vtk render window)
    windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
    windowToImageFilter->ReadFrontBufferOff(); // read from the back buffer
    windowToImageFilter->Update();
    
    std::string stdFileName = fileName.toStdString();
    vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetFileName(stdFileName.c_str());
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    writer->Write();
}