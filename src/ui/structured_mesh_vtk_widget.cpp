#include "include/ui/structured_mesh_vtk_widget.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolygon.h>
#include <vtkCellArray.h>
#include <vtkExtractEdges.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkWorldPointPicker.h>
#include <vtkProperty.h>
#include <QList>

#include "include/ui/structured_mesh_dialog.h"
#include "include/utility/mouse_interactor.h"

// vtkStandardNewMacro(MouseInteractor);

StructuredMeshVTKWidget::StructuredMeshVTKWidget(QWidget *parent) : QVTKWidget(parent) {
//    polyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
//    actor = vtkSmartPointer<vtkActor>::New();
//    renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
//    renderer = vtkSmartPointer<vtkRenderer>::New();
}

StructuredMeshVTKWidget::~StructuredMeshVTKWidget() {
//    renderer->Delete();
//    actor->Delete();
//    renderWindow->Delete();
//    actor->Delete();
//    polyDataMapper->Delete();
}

void StructuredMeshVTKWidget::setMesh(StructuredMesh *mesh) {
    if (mesh == NULL) {
        return;
    }

    MeshPolygon *boundaryPolygon = mesh->getBoundaryPolygon();

    if (boundaryPolygon->getFilteredPolygon() == NULL) {
        return;
    }

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();

    int count = 0;
    vtkPolygon *vtkBoundaryPolygon = mesh->getBoundaryPolygon()->getFilteredPolygon();

    for (vtkIdType i = 0; i < vtkBoundaryPolygon->GetNumberOfPoints(); i++) {
        points->InsertNextPoint(vtkBoundaryPolygon->GetPoints()->GetPoint(i));
        vtkBoundaryPolygon->GetPointIds()->SetId(i, count);
        count++;
    }

    polygons->InsertNextCell(vtkBoundaryPolygon);

    QList<MeshPolygon*> islands = mesh->getIslands();

    for (QList<MeshPolygon*>::const_iterator it = islands.begin(); it != islands.end(); it++) {
        vtkPolygon *islandsPolygon = (*it)->getFilteredPolygon();

        for (vtkIdType i = 0; i < islandsPolygon->GetNumberOfPoints(); i++) {
            points->InsertNextPoint(islandsPolygon->GetPoints()->GetPoint(i));
            islandsPolygon->GetPointIds()->SetId(i, count);
            count++;
        }

        polygons->InsertNextCell(islandsPolygon);
    }

    vtkSmartPointer<vtkPolyData> domainPolyData = vtkSmartPointer<vtkPolyData>::New();
    domainPolyData->SetPoints(points);
    domainPolyData->SetPolys(polygons);

    vtkSmartPointer<vtkExtractEdges> domainEdges = vtkSmartPointer<vtkExtractEdges>::New();
    domainEdges->SetInputData(domainPolyData);
    domainEdges->Update();

    vtkSmartPointer<vtkPolyData> gridPolyData = mesh->getGrid();
    vtkSmartPointer<vtkPolyDataMapper> domainMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    domainMapper->SetInputData(domainEdges->GetOutput());
    
    vtkSmartPointer<vtkPolyDataMapper> gridMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    gridMapper->SetInputData(gridPolyData);

    vtkSmartPointer<vtkActor> domainActor = vtkSmartPointer<vtkActor>::New();
    domainActor->SetMapper(domainMapper);
    domainActor->GetProperty()->EdgeVisibilityOn();

    vtkSmartPointer<vtkActor> gridActor = vtkSmartPointer<vtkActor>::New();
    gridActor->SetMapper(gridMapper);
    gridActor->GetProperty()->SetColor(1, 1, 1);
    gridActor->GetProperty()->EdgeVisibilityOn();

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();

    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkWorldPointPicker> worldPointPicker = vtkSmartPointer<vtkWorldPointPicker>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetPicker(worldPointPicker);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkSmartPointer<MouseInteractor> mouseInteractor = vtkSmartPointer<MouseInteractor>::New();
    mouseInteractor->SetDefaultRenderer(renderer);
    
    StructuredMeshDialog *parent = (StructuredMeshDialog*) this->parent();
    QObject::connect(mouseInteractor, SIGNAL(coordinateChanged(double&, double&)), parent, SLOT(setCoordinate(double&, double&)));
    parent->setArea(mesh->area());

    renderWindowInteractor->SetInteractorStyle(mouseInteractor);

    renderer->AddActor(gridActor);
    renderer->AddActor(domainActor);
    renderer->SetBackground(1, 1, 1);

    this->SetRenderWindow(renderWindow);

    renderWindow->Render();
}
