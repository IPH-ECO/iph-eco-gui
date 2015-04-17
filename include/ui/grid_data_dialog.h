#ifndef GRID_DATA_DIALOG_H
#define GRID_DATA_DIALOG_H

#include <QDialog>
#include <QSettings>

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
    void on_btnAddGridInfomation_clicked();
    void on_btnRemoveGridInformation_clicked();
    void on_rdoPoint_toggled(bool checked);
    void on_rdoPolygon_toggled(bool checked);
    void on_btnBrowse_clicked();
    void on_btnDone_clicked();

private:
    const QString GRID_DATA_DEFAULT_DIR_KEY;

    Ui::GridDataDialog *ui;
    QSettings *appSettings;
    GridDataConfiguration *unsavedGridDataConfiguration;
    GridDataConfiguration *currentGridDataConfiguration;
    GridData *unsavedGridData;
    GridData *currentGridData;

    QString getDefaultDirectory();
    void resetForm();
};

#endif // GRID_DATA_DIALOG_H
