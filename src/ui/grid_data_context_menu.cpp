#include <ui/grid_data_context_menu.h>

#include <ui/grid_data_dialog.h>
#include <ui/cell_update_dialog.h>
#include <ui/layer_properties_dialog.h>

#include <QIcon>

GridDataContextMenu::GridDataContextMenu(QWidget *parent) : QMenu(parent) {
    vtkWidget = static_cast<GridDataDialog*>(parent)->getVTKWidget();
    editGridLayerAttributesAction = new QAction(QIcon(":/icons/format-list-unordered.png"), tr("E&dit grid layer attributes"), this);
    editWeightsAction = new QAction(QIcon(":/icons/edit-clear.png"), tr("&Edit interpolated values"), this);
    
    addAction(editGridLayerAttributesAction);
    addAction(editWeightsAction);
    
    QObject::connect(editGridLayerAttributesAction, SIGNAL(triggered()), this, SLOT(editGridLayerAttributes()));
    QObject::connect(editWeightsAction, SIGNAL(triggered()), this, SLOT(editWeights()));
}

void GridDataContextMenu::toggleGridLayerAttributesAction(bool toggle) {
    editGridLayerAttributesAction->setEnabled(toggle);
}

void GridDataContextMenu::toggleEditWeightsAction(bool toggle) {
    editWeightsAction->setEnabled(toggle);
}

void GridDataContextMenu::editGridLayerAttributes() {
    LayerPropertiesDialog *dialog = new LayerPropertiesDialog(this->parentWidget(), vtkWidget->currentGridData->getLayerProperties());

    QObject::connect(dialog, SIGNAL(applyChanges()), vtkWidget, SLOT(render()));
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    dialog->exec();
}

void GridDataContextMenu::editWeights() {
    CellUpdateDialog *cellUpdateDialog = new CellUpdateDialog(vtkWidget);
    cellUpdateDialog->exec();
}
