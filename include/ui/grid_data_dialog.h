#ifndef GRID_DATA_DIALOG_H
#define GRID_DATA_DIALOG_H

#include <QDialog>
#include <QSettings>

#include "include/domain/grid_data.h"

namespace Ui {
class GridDataDialog;
}

class GridDataDialog : public QDialog {
    Q_OBJECT

public:
    explicit GridDataDialog(QWidget *parent = 0);
    ~GridDataDialog();

private slots:
    void on_btnBathymetry_clicked();
    void on_btnWindReduction_clicked();
    void on_btnChezy_clicked();
    void on_btnWetlandAreas_clicked();
    void on_cbxMesh_currentIndexChanged(const QString &arg1);

private:
    const QString GRID_DATA_DEFAULT_DIR_KEY;

    Ui::GridDataDialog *ui;
    QSettings *appSettings;
    GridData *unsavedGridData;
    GridData *currentGridData;

    QString getDefaultDirectory();
};

#endif // GRID_DATA_DIALOG_H
