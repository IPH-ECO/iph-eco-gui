#ifndef GRID_DATA_VTK_WIDGET_H
#define GRID_DATA_VTK_WIDGET_H

#include "include/domain/grid_data.h"
#include "include/utility/grid_data_mouse_interactor.h"

#include <QColor>
#include <QWidget>
#include <vtkActor.h>
#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkContextActor.h>
#include <vtkCubeAxesActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkScalarBarActor.h>

class GridDataVTKWidget : public QVTKWidget {
	Q_OBJECT
private:
	vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkActor> meshActor;
    vtkSmartPointer<vtkCubeAxesActor> axesActor;
    vtkSmartPointer<vtkActor> gridDataActor;
    vtkSmartPointer<vtkActor> colorMapActor;
    vtkSmartPointer<vtkActor> selectionActor;
    vtkSmartPointer<vtkPolyDataMapper> meshMapper;
    vtkSmartPointer<vtkScalarBarActor> inputPointsBar;
    vtkSmartPointer<vtkScalarBarActor> colorMapBar;
    vtkSmartPointer<GridDataMouseInteractor> mouseInteractor;
    
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
};

#endif // GRID_DATA_VTK_WIDGET_H
