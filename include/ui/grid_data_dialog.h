#ifndef GRID_DATA_DIALOG_H
#define GRID_DATA_DIALOG_H

#include <QDialog>
#include <QSettings>
#include <QWidget>

#include "include/domain/grid_data_configuration.h"

namespace Ui {
class GridDataDialog;
}

class GridDataDialog : public QDialog {
    Q_OBJECT

public:
    explicit GridDataDialog(QWidget *parent = 0);
    ~GridDataDialog();

private slots:
    void on_cbxMesh_currentIndexChanged(const QString &meshName);
    void on_cbxConfiguration_currentIndexChanged(const QString &configurationName);
    void on_btnAddGridInfomation_clicked();
    void on_btnRemoveGridInformation_clicked();
    void on_rdoPoint_toggled(bool checked);
    void on_rdoPolygon_toggled(bool checked);
    void on_btnBrowseInputFile_clicked();
    void on_btnSaveGridInformation_clicked();
    void on_tblGridInformation_itemSelectionChanged();
    void on_btnSaveConfiguration_clicked();
    void on_btnSaveAsNewConfiguration_clicked();
    void on_btnRemoveConfiguration_clicked();

    void on_btnDoneConfiguration_clicked();

private:
    const QString GRID_DATA_DEFAULT_DIR_KEY;

    Ui::GridDataDialog *ui;
    QSettings *appSettings;
    GridDataConfiguration *unsavedGridDataConfiguration;
    GridDataConfiguration *currentGridDataConfiguration;
    GridData *unsavedGridData;
    GridData *currentGridData;

    QString getDefaultDirectory();
    QWidget* createCheckBoxWidget(GridData *gridData);
    void resetGridDataForm();
    void enableGridDataConfigurationForm(bool enable);
    bool isConfigurationValid();
};

#endif // GRID_DATA_DIALOG_H
