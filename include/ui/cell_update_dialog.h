#ifndef CELL_UPDATE_DIALOG_H
#define CELL_UPDATE_DIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QSet>
#include <QList>

#include "grid_data_vtk_widget.h"

class GridDataVTKWidget;

namespace Ui {
class CellUpdateDialog;
}

class CellUpdateDialog : public QDialog {
    Q_OBJECT
private:
    Ui::CellUpdateDialog *ui;
    GridDataVTKWidget *gridDataVTKWidget;
    
    void fillGridValuesTable();
private slots:
    void on_btnSelectAll_clicked();
    void on_btnUpdate_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
public:
    explicit CellUpdateDialog(GridDataVTKWidget *gridDataVTKWidget);
    ~CellUpdateDialog();
};

#endif // CELL_UPDATE_DIALOG_H
