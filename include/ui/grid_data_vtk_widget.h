#ifndef GRID_DATA_VTK_WIDGET_H
#define GRID_DATA_VTK_WIDGET_H

#include "include/domain/grid_data.h"
#include "include/utility/grid_data_mouse_interactor.h"
#include "cell_update_dialog.h"
#include "grid_data_context_menu.h"

#include <QWidget>
#include <QMouseEvent>
#include <QVTKWidget.h>
#include <vtkCubeAxesActor.h>
#include <vtkScalarBarActor.h>
#include <vtkWorldPointPicker.h>
#include <vtkRenderWindowInteractor.h>

class GridDataContextMenu;
class CellUpdateDialog;

enum class CellLabelType { ID = 1, WEIGHT, UNDEFINED };

class GridDataVTKWidget : public QVTKWidget {
	Q_OBJECT
    
    friend class GridDataContextMenu;
    friend class CellUpdateDialog;
    
private:
	vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkSmartPointer<GridDataMouseInteractor> mouseInteractor;
    
    vtkSmartPointer<vtkActor> meshActor;
    vtkSmartPointer<vtkActor2D> cellLabelsActor;
    vtkSmartPointer<vtkCubeAxesActor> axesActor;
    vtkSmartPointer<vtkActor> gridDataActor;
    vtkSmartPointer<vtkPolyDataMapper> meshMapper;
    vtkSmartPointer<vtkScalarBarActor> inputPointsBar;
    vtkSmartPointer<vtkScalarBarActor> colorMapBar;
    
    vtkSmartPointer<vtkIdTypeArray> selectedCellIds;
    Mesh *currentMesh;
    GridData *currentGridData;
    
    bool showMesh;
    bool showAxes;
    bool showGridDataPoints;
    bool showColorMap;
    bool isCellPickActivated;

private slots:
    void handleMouseEvent(QMouseEvent *event);
    
public:
    explicit GridDataVTKWidget(QWidget *parent);
    void render(Mesh *mesh);
    void render(GridData *gridData);
    void changeBackgroundColor(const double &r, const double &g, const double &b);
    void clear();
    void toggleCellPick(bool activate, const CellPickMode &cellPickMode = CellPickMode::UNDEFINED);
    void toggleCellLabels(const CellLabelType &cellLabelType = CellLabelType::UNDEFINED);
    void lockView(bool lock);
    void exportToImage(const QString &fileName);
public slots:
    void setShowMesh(bool show);
    void setShowAxes(bool show);
    void setShowGridDataPoints(bool show);
    void setShowColorMap(bool show);
    void toggleZoomArea(bool activate);
    void resetZoom();
};

#endif // GRID_DATA_VTK_WIDGET_H
