#include "include/ui/simulation_vtk_widget.h"

#include "include/ui/main_window.h"
#include "include/exceptions/simulation_exception.h"

#include <QFileInfo>
#include <vtkLookupTable.h>
#include <vtkProperty.h>
#include <vtkFieldData.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkDoubleArray.h>
#include <vtkGeometryFilter.h>

SimulationVTKWidget::SimulationVTKWidget(QWidget *parent) : MeshVTKWidget(parent) {}

void SimulationVTKWidget::render(Simulation *simulation, const QString &layer, int frame) {
    QFileInfoList outputFiles = simulation->getOutputFiles();
    QFileInfoList outputFiles2;
    
    for (int i = outputFiles.size() - 1; i >= 0; i--) {
        outputFiles2.append(outputFiles.at(i));
    }
    
    if (layer.isEmpty()) {
        return;
    }
    
    if (frame >= outputFiles.size()) {
        throw SimulationException("Layer out of range.");
    }
    
    clear();
    
    std::string filename = outputFiles2.at(frame).absoluteFilePath().toStdString();
    std::string arrayName = layer.toStdString();
    
    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
	reader->SetFileName(filename.c_str());
	reader->Update();
    
    vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid = reader->GetUnstructuredGridOutput();
    double *range = unstructuredGrid->GetCellData()->GetArray(arrayName.c_str())->GetRange();
    vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    
    mapper->SetInputData(unstructuredGrid);
    mapper->SetScalarModeToUseCellData();
    mapper->SelectColorArray(arrayName.c_str());
    mapper->SetScalarRange(range[0], range[1]);
    
    meshActor = vtkSmartPointer<vtkActor>::New();
    meshActor->SetMapper(mapper);

	renderer->AddActor(meshActor);
    renderer->ResetCamera();
    
    MainWindow *mainWindow = static_cast<MainWindow*>(this->topLevelWidget());
    QObject::connect(mouseInteractor, SIGNAL(coordinateChanged(double&, double&)), mainWindow, SLOT(setCoordinate(double&, double&)));
}