#ifndef HYDRODYNAMIC_VTK_WIDGET_H
#define HYDRODYNAMIC_VTK_WIDGET_H

#include "include/domain/hydrodynamic_configuration.h"
#include "include/utility/hydrodynamic_mouse_interactor.h"

#include <QMouseEvent>
#include <QVTKWidget.h>
#include <vtkActor2D.h>
#include <vtkIdTypeArray.h>
#include <vtkCubeAxesActor.h>
#include <vtkWorldPointPicker.h>
#include <vtkRenderWindowInteractor.h>

class HydrodynamicVTKWidget : public QVTKWidget {
    Q_OBJECT
private:
	vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkSmartPointer<HydrodynamicMouseInteractor> mouseInteractor;

    vtkSmartPointer<vtkActor> meshActor;
    vtkSmartPointer<vtkCubeAxesActor> axesActor;
    vtkSmartPointer<vtkActor2D> labelsActor;

    vtkSmartPointer<vtkIdTypeArray> selectedCellIds;

    Mesh *currentMesh;
    
    bool showAxes;
    bool showMesh;
    bool isCellPickActivated;
private slots:
    void handleMouseEvent(QMouseEvent *event);
public:
    explicit HydrodynamicVTKWidget(QWidget *parent);
	void render(HydrodynamicConfiguration *hydrodynamicConfiguration);
    void togglePicker(bool activate, const PickerMode &pickerMode = PickerMode::NO_PICKER);
    vtkIdTypeArray* getSelectedCellIds() const;
    HydrodynamicMouseInteractor* getMouseInteractor() const;

public slots:
    void resetZoom();
    void toggleZoomArea(bool activate);
    void toggleAxes(bool show);
    void toggleMesh(bool show);
    void changeBackgroundColor(const double &r, const double &g, const double &b);
};

#endif // HYDRODYNAMIC_VTK_WIDGET_H