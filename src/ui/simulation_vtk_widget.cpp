#include "include/ui/simulation_vtk_widget.h"

#include "include/ui/main_window.h"

#include <vtkProperty.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericDataObjectReader.h>

SimulationVTKWidget::SimulationVTKWidget(QWidget *parent) : MeshVTKWidget(parent) {}

void SimulationVTKWidget::render(const QString &meshVTKFile) {
    clear();
    
	vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
	std::string filename = meshVTKFile.toStdString();

	reader->SetFileName(filename.c_str());
	reader->Update();

	vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputConnection(reader->GetOutputPort());
    
    meshActor = vtkSmartPointer<vtkActor>::New();
    meshActor->SetMapper(mapper);
    meshActor->GetProperty()->SetRepresentationToPoints();
    meshActor->GetProperty()->SetColor(0, 0, 0);

	renderer->AddActor(meshActor);
    renderer->AddActor(axesActor);
    renderer->ResetCamera();
    
    MainWindow *mainWindow = static_cast<MainWindow*>(this->topLevelWidget());
    QObject::connect(mouseInteractor, SIGNAL(coordinateChanged(double&, double&)), mainWindow, SLOT(setCoordinate(double&, double&)));
}