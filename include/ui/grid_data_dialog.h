#ifndef GRID_DATA_DIALOG_H
#define GRID_DATA_DIALOG_H

#include <domain/grid_data_configuration.h>
#include "grid_data_vtk_widget.h"
#include "abstract_mesh_dialog.h"


#include <QDialog>
#include <QWidget>
#include <QSettings>
#include <QTableWidgetItem>

namespace Ui {
class GridDataDialog;
}

class GridDataDialog : public AbstractMeshDialog {
    Q_OBJECT

public:
    explicit GridDataDialog(QWidget *parent = 0);
    ~GridDataDialog();

    GridDataVTKWidget* getVTKWidget();
    virtual void showEvent(QShowEvent *event);

private slots:
    void on_cbxMesh_currentIndexChanged(const QString &meshName);
    void on_cbxConfiguration_currentIndexChanged(const QString &configurationName);
    void on_btnAddGridLayer_clicked();
    void on_btnEditGridLayer_clicked();
    void on_btnRemoveGridLayer_clicked();
    void on_btnNewConfiguration_clicked();
    void on_btnApplyConfiguration_clicked();
    void on_btnRemoveConfiguration_clicked();
    void on_tblGridLayers_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_tblGridLayers_cellDoubleClicked(int row, int column);
    void onPickIndividualCellAction(bool checked);
    void onPickCellSetAction(bool checked);
    void onShowCellWeightsAction(bool checked);
    void onZoomAreaAction(bool checked);
//    void on_btnLockView_clicked(bool checked);

private:
    const QString GRID_DATA_DEFAULT_DIR_KEY;
    
    Ui::GridDataDialog *ui;
    GridDataConfiguration *unsavedConfiguration;
    GridDataConfiguration *currentConfiguration;
    Mesh *currentMesh;
    QSettings *appSettings;
    QAction *pickIndividualCellAction;
    QAction *pickCellSetAction;
    QAction *showGridDataPointsAction;
    QAction *showColorMapAction;
    QAction *showCellWeightsAction;
    
    bool isConfigurationValid(const QString &configurationName);
    void showGridLayerDialog(GridData *gridData);
    QString getDefaultDirectory();
};

#endif // GRID_DATA_DIALOG_H
