#ifndef GRID_DATA_DIALOG_H
#define GRID_DATA_DIALOG_H

#include "include/domain/grid_data_configuration.h"
#include "grid_data_vtk_widget.h"

#include <QDialog>
#include <QWidget>
#include <QSettings>
#include <QTableWidgetItem>

namespace Ui {
class GridDataDialog;
}

class GridDataDialog : public QDialog {
    Q_OBJECT

public:
    explicit GridDataDialog(QWidget *parent = 0);
    ~GridDataDialog();

    void setArea(const double &area);
    GridDataVTKWidget* getGridDataVTKWidget() const;

public slots:
    void setCoordinate(double &x, double &y);

private slots:
    void on_cbxMesh_currentIndexChanged(const QString &meshName);
    void on_cbxConfiguration_currentIndexChanged(const QString &configurationName);
    void on_btnAddGridLayer_clicked();
    void on_btnEditGridLayer_clicked();
    void on_btnRemoveGridLayer_clicked();
    void on_btnSaveConfiguration_clicked();
    void on_btnRemoveConfiguration_clicked();
    void on_btnDoneConfiguration_clicked();
    void on_tblGridLayers_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_tblGridLayers_cellDoubleClicked(int row, int column);
    void on_btnBackgroundColor_clicked();
    void on_btnPickIndividualCells_clicked(bool checked);
    void on_btnPickCellSet_clicked(bool checked);
    void on_btnShowCellLabels_clicked(bool checked);
    void on_btnShowCellWeights_clicked(bool checked);
    void on_btnLockView_clicked(bool checked);
    void on_btnExport_clicked();

private:
    const QString GRID_DATA_DEFAULT_DIR_KEY;
    
    Ui::GridDataDialog *ui;
    GridDataConfiguration *unsavedConfiguration;
    GridDataConfiguration *currentConfiguration;
    Mesh *currentMesh;
    QSettings *appSettings;
    
    void toggleGridDataConfigurationForm(bool enable);
    bool isConfigurationValid(const QString &configurationName);
    void showGridLayerDialog(GridData *gridData);
    QString getDefaultDirectory();
};

#endif // GRID_DATA_DIALOG_H
