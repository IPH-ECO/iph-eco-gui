#ifndef GRID_DATA_VTK_WIDGET_H
#define GRID_DATA_VTK_WIDGET_H

#include "include/domain/grid_data.h"
#include "include/utility/grid_data_mouse_interactor.h"

#include <QWidget>
#include <QMouseEvent>
#include <QVTKWidget.h>
#include <vtkCubeAxesActor.h>
#include <vtkScalarBarActor.h>
#include <vtkWorldPointPicker.h>
#include <vtkRenderWindowInteractor.h>

class GridDataVTKWidget : public QVTKWidget {
	Q_OBJECT
private:
	vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkWorldPointPicker> worldPointPicker;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkSmartPointer<GridDataMouseInteractor> mouseInteractor;
    
    vtkSmartPointer<vtkActor> meshActor;
    vtkSmartPointer<vtkCubeAxesActor> axesActor;
    vtkSmartPointer<vtkActor> gridDataActor;
    vtkSmartPointer<vtkPolyDataMapper> meshMapper;
    vtkSmartPointer<vtkScalarBarActor> inputPointsBar;
    vtkSmartPointer<vtkScalarBarActor> colorMapBar;
    
    Mesh *currentMesh;
    GridData *currentGridData;
    
    bool showMesh;
    bool showAxes;
    bool showGridDataPoints;
    bool showColorMap;
public:
    GridDataVTKWidget(QWidget *parent);
    void render(Mesh *mesh);
    void render(GridData *gridData);
    void changeBackgroundColor(const double &r, const double &g, const double &b);
    void clear();
public slots:
    void setShowMesh(bool show);
    void setShowAxes(bool show);
    void setShowGridDataPoints(bool show);
    void setShowColorMap(bool show);
    void togglePickIndividualCell(bool activate);
    void handleMouseEvent(QMouseEvent *event);
};

#endif // GRID_DATA_VTK_WIDGET_H
