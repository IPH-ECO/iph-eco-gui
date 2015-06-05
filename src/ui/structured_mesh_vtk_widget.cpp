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
    MeshPolygon *boundaryPolygon = mesh->getBoundaryPolygon();

    if (boundaryPolygon->getFilteredPolygon() == NULL) {
        return;
    }

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(1, 1, 1);

    if (mesh->getShowBoundaryEdges()) {
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

        vtkSmartPointer<vtkActor> domainActor = vtkSmartPointer<vtkActor>::New();
        domainActor->SetMapper(domainMapper);
        domainActor->GetProperty()->EdgeVisibilityOn();

        renderer->AddActor(domainActor);
    }

    if (mesh->getShowMesh()) {
        vtkSmartPointer<vtkPolyData> gridPolyData = mesh->getGrid();
        vtkSmartPointer<vtkPolyDataMapper> gridMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        vtkSmartPointer<vtkActor> gridActor = vtkSmartPointer<vtkActor>::New();

        gridMapper->SetInputData(gridPolyData);
        gridActor->SetMapper(gridMapper);
        gridActor->GetProperty()->SetColor(1, 1, 1);
        gridActor->GetProperty()->EdgeVisibilityOn();
        renderer->AddActor(gridActor);
    }

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkWorldPointPicker> worldPointPicker = vtkSmartPointer<vtkWorldPointPicker>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<MouseInteractor> mouseInteractor = vtkSmartPointer<MouseInteractor>::New();
    
    mouseInteractor->SetDefaultRenderer(renderer);
    
    StructuredMeshDialog *parent = (StructuredMeshDialog*) this->parent();
    QObject::connect(mouseInteractor, SIGNAL(coordinateChanged(double&, double&)), parent, SLOT(setCoordinate(double&, double&)));
    parent->setArea(mesh->area());

    renderWindowInteractor->SetInteractorStyle(mouseInteractor);
    renderWindowInteractor->SetPicker(worldPointPicker);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    
    renderWindow->AddRenderer(renderer);

    this->SetRenderWindow(renderWindow);
    renderWindow->Render();
}
