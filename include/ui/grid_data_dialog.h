#ifndef GRID_DATA_DIALOG_H
#define GRID_DATA_DIALOG_H

#include <QDialog>
#include <QWidget>
#include <QTableWidgetItem>

#include "include/domain/grid_data_configuration.h"
#include "include/ui/grid_information_dialog.h"

namespace Ui {
class GridDataDialog;
}

class GridDataDialog : public QDialog {
    Q_OBJECT

public:
    explicit GridDataDialog(QWidget *parent = 0);
    ~GridDataDialog();

    void setArea(const double &area);

public slots:
    void setCoordinate(double &x, double &y);

private slots:
    void on_cbxMesh_currentIndexChanged(const QString &meshName);
    void on_cbxConfiguration_currentIndexChanged(const QString &configurationName);
    void on_btnAddGridInfomation_clicked();
    void on_btnRemoveGridInformation_clicked();
    void on_btnSaveConfiguration_clicked();
    void on_btnSaveAsNewConfiguration_clicked();
    void on_btnRemoveConfiguration_clicked();
    void on_btnDoneConfiguration_clicked();
    void on_tblGridInformation_itemDoubleClicked(QTableWidgetItem *item);
    void on_btnShowGridDataPoints_clicked(bool checked);
    void on_btnShowInterpolationResult_clicked(bool checked);
    void on_btnShowMesh_clicked(bool checked);

private:
    Ui::GridDataDialog *ui;
    GridDataConfiguration *unsavedConfiguration;
    GridDataConfiguration *currentConfiguration;
    Mesh *currentMesh;
    
    void toggleGridDataConfigurationForm(bool enable);
    bool isConfigurationValid(const QString &configurationName);
};

#endif // GRID_DATA_DIALOG_H
