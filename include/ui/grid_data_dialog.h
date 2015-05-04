#ifndef GRID_DATA_DIALOG_H
#define GRID_DATA_DIALOG_H

#include <QDialog>
#include <QSettings>
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

    void setRealCoordinate(const Point &point);

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
    void on_chkShowMesh_toggled(bool checked);

private:
    Ui::GridDataDialog *ui;
    GridDataConfiguration *unsavedGridDataConfiguration;
    GridDataConfiguration *currentGridDataConfiguration;
    
    QWidget* createCheckBoxWidget(GridData *gridData);
    void toggleGridDataConfigurationForm(bool enable);
    bool isConfigurationValid();
};

#endif // GRID_DATA_DIALOG_H
