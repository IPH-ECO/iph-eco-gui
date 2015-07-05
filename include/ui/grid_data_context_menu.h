#ifndef GRID_DATA_CONTEXT_MENU_H
#define GRID_DATA_CONTEXT_MENU_H

#include "grid_data_vtk_widget.h"

#include <QWidget>
#include <QAction>
#include <QMenu>

class GridDataVTKWidget;

class GridDataContextMenu : public QMenu {
    Q_OBJECT
private:
    GridDataVTKWidget *vtkWidget;
    QAction *editWeightsAction;
    QAction *editGridLayerAttributesAction;
private slots:
    void editGridLayerAttributes();
    void editWeights();
public:
    explicit GridDataContextMenu(QWidget *parent);
    void toggleGridLayerAttributesAction(bool toggle);
    void toggleEditWeightsAction(bool toggle);
};

#endif // GRID_DATA_CONTEXT_MENU_H
