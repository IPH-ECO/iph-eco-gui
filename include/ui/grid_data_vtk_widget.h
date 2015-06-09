#ifndef GRID_DATA_VTK_WIDGET_H
#define GRID_DATA_VTK_WIDGET_H

#include <QWidget>
#include <QVTKWidget.h>
#include <vtkRenderer.h>

#include "include/domain/grid_data_configuration.h"

class GridDataVTKWidget : public QVTKWidget {
	Q_OBJECT
private:
	vtkSmartPointer<vtkRenderer> renderer;

public:
    GridDataVTKWidget(QWidget *parent);
    ~GridDataVTKWidget();
    void render(GridDataConfiguration *mesh);
    void clear();
};

#endif // GRID_DATA_VTK_WIDGET_H
