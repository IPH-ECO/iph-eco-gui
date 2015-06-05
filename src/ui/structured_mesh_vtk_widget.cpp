#include "include/ui/structured_mesh_vtk_widget.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
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

//    vtkPolyData *polyData = mesh->toPolyData();

//    if (polyData == NULL) {
//        return;
//    }

    MeshPolygon *boundaryPolygon = mesh->getBoundaryPolygon();

    if (boundaryPolygon->getFilteredPolygon() == NULL) {
        return;
    }

    // vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
    // triangleFilter->SetInputData(polyData);
    // triangleFilter->Update();

    // vtkSmartPointer<vtkPolyData> pd2 = triangleFilter->GetOutput();

    // QList<MeshPolygon*> domain = mesh->getDomain();
    // vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    // vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();

    // int i = 0;

    // for (QList<MeshPolygon*>::const_iterator it = domain.begin(); it != domain.end(); it++) {
    //     vtkSmartPointer<vtkPolygon> polygon = (*it)->getFilteredPolygon();
    //     int numberOfPoints = polygon->GetPoints()->GetNumberOfPoints();
    //     double p[3] = { 174839, 8930064, 0.0 };

    //     polygon->GetPointIds()->SetNumberOfIds(numberOfPoints);

    //     qDebug() << (*it)->pointInPolygon(p);

    //     for (vtkIdType j = 0; j < numberOfPoints; j++) {
    //         points->InsertNextPoint(polygon->GetPoints()->GetPoint(j));
    //         polygon->GetPointIds()->SetId(j, i);
    //         i++;
    //     }

    //     polygons->InsertNextCell(polygon);
    // }

    // vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    // polyData->SetPoints(points);
    // polyData->SetPolys(polygons);

    // vtkSmartPointer<vtkFeatureEdges> edges = vtkSmartPointer<vtkFeatureEdges>::New();
    // edges->SetInputData(polyData);
    // edges->BoundaryEdgesOn();
    // edges->FeatureEdgesOff();
    // edges->ManifoldEdgesOff();
    // edges->NonManifoldEdgesOff();
    // edges->Update();

    // vtkSmartPointer<vtkPolyDataMapper> polyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    // polyDataMapper->SetInputData(edges->GetOutput());

    vtkSmartPointer<vtkPolyData> polyData = mesh->getGrid();

    // vtkSmartPointer<vtkExtractEdges> extractEdges = vtkSmartPointer<vtkExtractEdges>::New();
    // extractEdges->SetInputData(polyData);
    // extractEdges->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);
    // mapper->SetInputConnection(extractEdges->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 1, 1);
    actor->GetProperty()->EdgeVisibilityOn();

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

    renderer->AddActor(actor);
    renderer->SetBackground(1, 1, 1);

    this->SetRenderWindow(renderWindow);

    renderWindow->Render();
}
