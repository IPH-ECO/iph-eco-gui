#ifndef GRID_INFORMATION_DIALOG_H
#define GRID_INFORMATION_DIALOG_H

#include "include/domain/grid_data_configuration.h"
#include "include/domain/grid_data.h"

#include <QDialog>
#include <QSettings>
#include <QAbstractButton>

namespace Ui {
class GridInformationDialog;
}

class GridInformationDialog : public QDialog {
    Q_OBJECT

public:
    explicit GridInformationDialog(QDialog *parent, GridDataConfiguration *gridConfiguration, GridData *gridData, Mesh *mesh = nullptr);
    ~GridInformationDialog();
    GridData* getGridData();

private slots:
    void on_rdoPoint_toggled(bool checked);
    void on_rdoPolygon_toggled(bool checked);
    void on_btnBrowseInputFile_clicked();

private:
    const QString GRID_DATA_DEFAULT_DIR_KEY;

    Ui::GridInformationDialog *ui;
    QSettings *appSettings;
    GridDataConfiguration *gridConfiguration;
    GridData *gridData;
    Mesh *mesh;

    QString getDefaultDirectory();
    void toggleGridDataConfigurationForm(bool enable);
    bool isValid();
    virtual void accept();
};

#endif // GRID_INFORMATION_DIALOG_H
