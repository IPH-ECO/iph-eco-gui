#ifndef BOUNDARY_CONDITION_DIALOG_H
#define BOUNDARY_CONDITION_DIALOG_H

#include <QDialog>
#include <QWidget>

#include "hydrodynamic_data_dialog.h"

namespace Ui {
	class BoundaryConditionDialog;
}

class BoundaryConditionDialog : public QDialog {
	Q_OBJECT
private:
	Ui::BoundaryConditionDialog *ui;
    HydrodynamicDataDialog *hydrodynamicDataDialog;
public:
	explicit BoundaryConditionDialog(QWidget *parent);
    ~BoundaryConditionDialog();
    
    void setHydrodynamicDataDialog(HydrodynamicDataDialog *dialog);
private slots:
    void on_rdoWaterLevel_clicked(bool checked);
    void on_rdoWaterFlow_clicked(bool checked);
    void on_btnTimeSeries_clicked();
    void btnSingleCellPicker_clicked();
    void btnMultipleCellPicker_clicked();
};

#endif // BOUNDARY_CONDITION_DIALOG_H