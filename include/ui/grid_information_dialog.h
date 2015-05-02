#ifndef GRID_INFORMATION_DIALOG_H
#define GRID_INFORMATION_DIALOG_H

#include <QDialog>
#include <QSettings>
#include <QAbstractButton>

#include "include/domain/grid_data.h"

namespace Ui {
class GridInformationDialog;
}

class GridInformationDialog : public QDialog {
    Q_OBJECT

public:
    explicit GridInformationDialog(QDialog *parent, GridData *gridData);
    ~GridInformationDialog();
    GridData* getGridData();

private slots:
    void on_rdoPoint_toggled(bool checked);
    void on_rdoPolygon_toggled(bool checked);
    void on_btnBrowseInputFile_clicked();
    void on_bottomButtons_clicked(QAbstractButton *button);

private:
    const QString GRID_INFORMATION_DEFAULT_DIR_KEY;

    Ui::GridInformationDialog *ui;
    QSettings *appSettings;
    GridData *gridData;

    QString getDefaultDirectory();
    void toggleGridDataConfigurationForm(bool enable);
    bool isConfigurationValid();
};

#endif // GRID_INFORMATION_DIALOG_H
