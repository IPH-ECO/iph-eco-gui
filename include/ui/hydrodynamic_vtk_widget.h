#ifndef HYDRODYNAMIC_VTK_WIDGET_H
#define HYDRODYNAMIC_VTK_WIDGET_H

#include <domain/hydrodynamic_configuration.h>
#include <utility/hydrodynamic_mouse_interactor.h>
#include "mesh_vtk_widget.h"

#include <QMouseEvent>

class HydrodynamicVTKWidget : public MeshVTKWidget {
    Q_OBJECT
private:
    vtkSmartPointer<vtkIdTypeArray> selectedCellIds;
private slots:
    void handleMouseEvent(QMouseEvent *event);
public:
    explicit HydrodynamicVTKWidget(QWidget *parent);
	void render(HydrodynamicConfiguration *hydrodynamicConfiguration);
    void togglePicker(bool activate, const PickerMode &pickerMode = PickerMode::NO_PICKER);
    vtkIdTypeArray* getSelectedCellIds() const;
    HydrodynamicMouseInteractor* getMouseInteractor() const;
};

#endif // HYDRODYNAMIC_VTK_WIDGET_H