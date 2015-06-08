#include "include/ui/mesh_vtk_widget.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolygon.h>
#include <vtkCellArray.h>
#include <vtkExtractEdges.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkWorldPointPicker.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <QList>

#include "include/ui/structured_mesh_dialog.h"
#include "include/ui/unstructured_mesh_dialog.h"
#include "include/utility/mouse_interactor.h"

MeshVTKWidget::MeshVTKWidget(QWidget *parent) : QVTKWidget(parent) {}

MeshVTKWidget::~MeshVTKWidget() {}

void MeshVTKWidget::render(Mesh *mesh) {
    MeshPolygon *boundaryPolygon = mesh->getBoundaryPolygon();

    if (boundaryPolygon->getFilteredPolygon() == NULL) {
        return;
    }

    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(1, 1, 1);

    // Contour rendering
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();
    QList<MeshPolygon*> meshPolygons = mesh->getIslands();
    int count = 0;

    meshPolygons.prepend(mesh->getBoundaryPolygon());

    for (QList<MeshPolygon*>::const_iterator it = meshPolygons.begin(); it != meshPolygons.end(); it++) {
        vtkPolygon *vtkMeshPolygon = (*it)->getFilteredPolygon();

        for (vtkIdType i = 0; i < vtkMeshPolygon->GetPoints()->GetNumberOfPoints(); i++) {
            double point[3];

            vtkMeshPolygon->GetPoints()->GetPoint(i, point); // Safe call
            vtkMeshPolygon->GetPointIds()->SetId(i, count);
            points->InsertNextPoint(point);
            count++;
        }

        polygons->InsertNextCell(vtkMeshPolygon);
    }

    vtkSmartPointer<vtkPolyData> domainPolyData = vtkSmartPointer<vtkPolyData>::New();
    domainPolyData->SetPoints(points);
    domainPolyData->SetPolys(polygons);

    vtkSmartPointer<vtkExtractEdges> domainEdges = vtkSmartPointer<vtkExtractEdges>::New();
    domainEdges->SetInputData(domainPolyData);
    domainEdges->Update();

    vtkSmartPointer<vtkPolyDataMapper> domainMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    domainMapper->SetInputData(domainEdges->GetOutput());

    this->boundaryEdgesActor = vtkSmartPointer<vtkActor>::New();
    this->boundaryEdgesActor->SetMapper(domainMapper);
    this->boundaryEdgesActor->GetProperty()->EdgeVisibilityOn();

    if (!mesh->getShowBoundaryEdges()) {
        this->boundaryEdgesActor->VisibilityOff();
    }

    renderer->AddActor(this->boundaryEdgesActor);

    // Mesh rendering
    vtkSmartPointer<vtkPolyData> gridPolyData = mesh->getGrid();
    vtkSmartPointer<vtkPolyDataMapper> gridMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    gridMapper->SetInputData(gridPolyData);
    
    this->gridActor = vtkSmartPointer<vtkActor>::New();
    this->gridActor->SetMapper(gridMapper);
    // this->gridActor->GetProperty()->SetColor(1, 1, 1);
    this->gridActor->GetProperty()->EdgeVisibilityOn();

    if (!mesh->getShowMesh()) {
        this->gridActor->VisibilityOff();
    }

    renderer->AddActor(this->gridActor);

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkWorldPointPicker> worldPointPicker = vtkSmartPointer<vtkWorldPointPicker>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<MouseInteractor> mouseInteractor = vtkSmartPointer<MouseInteractor>::New();
    
    mouseInteractor->SetDefaultRenderer(renderer);
    
    if (mesh->instanceOf("StructuredMesh")) {
        StructuredMeshDialog *structuredMeshDialog = (StructuredMeshDialog*) this->parent();
        QObject::connect(mouseInteractor, SIGNAL(coordinateChanged(double&, double&)), structuredMeshDialog, SLOT(setCoordinate(double&, double&)));
        structuredMeshDialog->setArea(mesh->area());
    } else {
        UnstructuredMeshDialog *unstructuredMeshDialog = (UnstructuredMeshDialog*) this->parent();
        QObject::connect(mouseInteractor, SIGNAL(coordinateChanged(double&, double&)), unstructuredMeshDialog, SLOT(setCoordinate(double&, double&)));
        unstructuredMeshDialog->setArea(mesh->area());
    }

    renderWindowInteractor->SetInteractorStyle(mouseInteractor);
    renderWindowInteractor->SetPicker(worldPointPicker);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    
    renderWindow->AddRenderer(renderer);

    this->SetRenderWindow(renderWindow);
    renderWindow->Render();
}

void MeshVTKWidget::clear() {
    if (renderer != NULL) {
        renderer->RemoveAllViewProps();
        this->update();
    }
}

void MeshVTKWidget::showBoundaryEdges(const bool &show) {
    if (show) {
        this->boundaryEdgesActor->VisibilityOn();
    } else {
        this->boundaryEdgesActor->VisibilityOff();
    }
    this->update();
}

void MeshVTKWidget::showMesh(const bool &show) {
    if (show) {
        this->gridActor->VisibilityOn();
    } else {
        this->gridActor->VisibilityOff();
    }
    this->update();
}