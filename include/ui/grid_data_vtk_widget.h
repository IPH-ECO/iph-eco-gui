#ifndef GRID_DATA_VTK_WIDGET_H
#define GRID_DATA_VTK_WIDGET_H

#include "include/domain/grid_data.h"

#include <QWidget>
#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

class GridDataVTKWidget : public QVTKWidget {
	Q_OBJECT
private:
	vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkActor> meshActor;
    vtkSmartPointer<vtkActor> gridDataActor;
    vtkSmartPointer<vtkPolyDataMapper> meshMapper;
    
    bool showMesh;
    bool showGridDataPoints;
    bool showInterpolationResult;
public:
    GridDataVTKWidget(QWidget *parent);
    ~GridDataVTKWidget();
    void render(Mesh *mesh);
    void render(GridData *gridData);
    void setShowMesh(const bool &value);
    void setShowGridDataPoints(const bool &value);
    void setShowInterpolationResult(const bool &value);
    void clear();
};

#endif // GRID_DATA_VTK_WIDGET_H
