#include "include/ui/mesh_vtk_widget.h"

#include <vtkPolygon.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkCellArray.h>
#include <vtkAreaPicker.h>
#include <vtkSmartPointer.h>
#include <vtkExtractEdges.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkWorldPointPicker.h>
#include <vtkRenderWindowInteractor.h>
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
    vtkSmartPointer<vtkPolyData> meshPolyData = mesh->getMeshPolyData();
    vtkSmartPointer<vtkPolyDataMapper> meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    meshMapper->SetInputData(meshPolyData);
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
    this->boundaryEdgesActor->SetVisibility(show);
    this->update();
}

void MeshVTKWidget::toggleMesh(bool show) {
    this->showMesh = show;
    this->meshActor->SetVisibility(show);
    this->update();
}

void MeshVTKWidget::toggleAxes(bool show) {
    this->showAxes = show;
    this->axesActor->SetVisibility(show);
    this->update();
}