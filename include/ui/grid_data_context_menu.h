#ifndef GRID_DATA_CONTEXT_MENU_H
#define GRID_DATA_CONTEXT_MENU_H

#include "grid_data_vtk_widget.h"
#include "cell_update_dialog.h"

#include <QWidget>
#include <QAction>
#include <QMenu>
#include <QIcon>

class GridDataContextMenu : public QMenu {
    Q_OBJECT
private:
    QAction *editWeightsAction;
    
public:
    explicit GridDataContextMenu(QWidget *parent) : QMenu(parent) {
        editWeightsAction = new QAction(QIcon(":/icons/document-properties.png"), tr("&Edit cells weights"), this);
        
        addAction(editWeightsAction);
        
        QObject::connect(editWeightsAction, SIGNAL(triggered()), this, SLOT(editWeights()));
    }
    
private slots:
    void editWeights() {
        GridDataVTKWidget *parent = static_cast<GridDataVTKWidget*>(this->parentWidget());
        CellUpdateDialog *cellUpdateDialog = new CellUpdateDialog(parent);
        cellUpdateDialog->exec();
    }
};

#endif // GRID_DATA_CONTEXT_MENU_H
