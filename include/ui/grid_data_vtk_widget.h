#ifndef GRID_DATA_VTK_WIDGET_H
#define GRID_DATA_VTK_WIDGET_H

#include "include/domain/grid_data.h"
#include "include/utility/grid_data_mouse_interactor.h"
#include "cell_update_dialog.h"
#include "grid_data_context_menu.h"
#include "mesh_vtk_widget.h"

#include <QWidget>
#include <QMouseEvent>
#include <QVTKWidget.h>
#include <vtkCubeAxesActor.h>
#include <vtkScalarBarWidget.h>
#include <vtkWorldPointPicker.h>
#include <vtkColorTransferFunction.h>
#include <vtkRenderWindowInteractor.h>

class GridDataContextMenu;
class CellUpdateDialog;

class GridDataVTKWidget : public MeshVTKWidget {
	Q_OBJECT
    
    friend class GridDataContextMenu;
    friend class CellUpdateDialog;
    
private:
    vtkSmartPointer<GridDataMouseInteractor> gridDataMouseInteractor;
    
    vtkSmartPointer<vtkActor> mapActor;
    vtkSmartPointer<vtkScalarBarWidget> mapScalarBarWidget;
    vtkSmartPointer<vtkActor> mapPointsActor;
    vtkSmartPointer<vtkScalarBarWidget> pointsScalarBarWidget;
    
    vtkSmartPointer<vtkIdTypeArray> selectedCellIds;
    
    GridData *currentGridData;
    
    bool showMapPoints;
    bool showMap;
    bool isCellPickActivated;
    
    vtkColorTransferFunction* buildColorTransferFunction(bool isColorMap);

private slots:
    void handleMouseEvent(QMouseEvent *event);
    
public:
    explicit GridDataVTKWidget(QWidget *parent);
    void render(Mesh *mesh);
    void render(GridData *gridData);
    void clear();
    void toggleCellPick(bool activate, const PickerMode &pickerMode = PickerMode::NO_PICKER);
    void toggleCellLabels(const LabelType &labelType = LabelType::UNDEFINED);
    void lockView(bool lock);
public slots:
    void toggleMesh(bool show);
    void toggleMapPoints(bool show);
    void toggleMap(bool show);
};

#endif // GRID_DATA_VTK_WIDGET_H
