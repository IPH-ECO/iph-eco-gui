#ifndef VERTICAL_INTEGRATED_BOUNDARY_CONDITION_DIALOG_H
#define VERTICAL_INTEGRATED_BOUNDARY_CONDITION_DIALOG_H

#include <QDialog>
#include "hydrodynamic_data_dialog.h"
#include "hydrodynamic_boundary_condition_dialog.h"

namespace Ui {
	class VerticalIntegratedBoundaryConditionDialog;
}

class VerticalIntegratedBoundaryConditionDialog : public HydrodynamicBoundaryConditionDialog {
    Q_OBJECT
private:
    Ui::VerticalIntegratedBoundaryConditionDialog *ui;
    QSet<VerticalIntegratedRange*> unsavedRanges;
    
    void accept();
    bool isValid();
private slots:
    void on_btnAddRange_clicked();
    void on_btnRemoveRange_clicked();
    void onCbxFunctionCurrentTextChanged(const QString &text);
    void onBtnTimeSeriesClicked();
public:
    explicit VerticalIntegratedBoundaryConditionDialog(HydrodynamicConfiguration *configuration, HydrodynamicBoundaryCondition *boundaryCondition);
    virtual ~VerticalIntegratedBoundaryConditionDialog();
};

#endif // VERTICAL_INTEGRATED_BOUNDARY_CONDITION_DIALOG_H
