#include <ui/meteorological_vtk_widget.h>

#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkPNGReader.h>
#include <vtkImageData.h>
#include <vtkTextProperty.h>
#include <vtkDataSetMapper.h>
#include <vtkCaptionActor2D.h>
#include <vtkActorCollection.h>
#include <vtkImageChangeInformation.h>

MeteorologicalVTKWidget::MeteorologicalVTKWidget(QWidget *parent) : MeshVTKWidget(parent) {}

void MeteorologicalVTKWidget::addStation(MeteorologicalStation *station) {
    if (station->getIconActor()) {
        renderer->RemoveActor(station->getIconActor());
        renderer->RemoveActor(station->getCaptionActor());
    }
    
    vtkSmartPointer<vtkPNGReader> iconReader = vtkSmartPointer<vtkPNGReader>::New();
    iconReader->SetFileName("./station.png");
    iconReader->Update();
    
    vtkSmartPointer<vtkImageData> imageData = iconReader->GetOutput();
    imageData->SetOrigin(station->getUtmX(), station->getUtmY(), 0);
    
    vtkSmartPointer<vtkImageChangeInformation> imageResize = vtkSmartPointer<vtkImageChangeInformation>::New();
    imageResize->SetInputData(imageData);
    imageResize->SetOutputSpacing(2, 2, 1);
    imageResize->Update();
    
    vtkSmartPointer<vtkDataSetMapper> iconMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    iconMapper->SetInputData(imageResize->GetOutput());
    
    vtkSmartPointer<vtkActor> iconActor = vtkSmartPointer<vtkActor>::New();
    iconActor->SetMapper(iconMapper);
    station->setIconActor(iconActor);
    
    vtkSmartPointer<vtkCaptionActor2D> captionActor = vtkSmartPointer<vtkCaptionActor2D>::New();
    captionActor->SetAttachmentPoint(station->getUtmX(), station->getUtmY(), 0);
    captionActor->SetCaption(station->getName().toStdString().c_str());
    captionActor->BorderOff();
    station->setCaptionActor(captionActor);
    
    renderer->AddActor(captionActor);
    renderer->AddActor(iconActor);
    this->update();
}

void MeteorologicalVTKWidget::removeStation(MeteorologicalStation *station) {
    renderer->RemoveActor(station->getIconActor());
    renderer->RemoveActor(station->getCaptionActor());
    this->update();
}

void MeteorologicalVTKWidget::highlightStation(MeteorologicalStation *station) {
    vtkSmartPointer<vtkActorCollection> iconActors = renderer->GetActors();
    vtkSmartPointer<vtkActor> highlightedActor = station->getIconActor();
    
    iconActors->InitTraversal();
    
    vtkSmartPointer<vtkActor> iconActor = iconActors->GetNextActor();
    
    while (iconActor) {
        if (iconActor == highlightedActor) {
            iconActor->GetProperty()->SetEdgeColor(1.0, 1.0, 0.0);
            iconActor->GetProperty()->SetLineWidth(1);
        } else {
            iconActor->GetProperty()->SetLineWidth(0);
        }
        iconActor = iconActors->GetNextActor();
    }
    
    this->update();
}

void MeteorologicalVTKWidget::clear() {
    renderer->RemoveAllViewProps();
    this->update();
}