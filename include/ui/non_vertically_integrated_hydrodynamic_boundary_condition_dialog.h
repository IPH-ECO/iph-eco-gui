#ifndef NON_VERTICALLY_INTEGRATED_HYDRODYNAMIC_BOUNDARY_CONDITION_DIALOG_H
#define NON_VERTICALLY_INTEGRATED_HYDRODYNAMIC_BOUNDARY_CONDITION_DIALOG_H

#include <QDialog>
#include "hydrodynamic_data_dialog.h"
#include "hydrodynamic_boundary_condition_dialog.h"

namespace Ui {
	class NonVerticallyIntegratedHydrodynamicBoundaryConditionDialog;
}

class NonVerticallyIntegratedHydrodynamicBoundaryConditionDialog : public HydrodynamicBoundaryConditionDialog {
    Q_OBJECT
private:
    Ui::NonVerticallyIntegratedHydrodynamicBoundaryConditionDialog *ui;
    QSet<NonVerticallyIntegratedRange*> unsavedRanges;
    
    void accept();
    bool isValid();
private slots:
    void on_btnAddRange_clicked();
    void on_btnRemoveRange_clicked();
    void onCbxFunctionCurrentTextChanged(const QString &text);
    void onBtnTimeSeriesClicked();
public:
    explicit NonVerticallyIntegratedHydrodynamicBoundaryConditionDialog(HydrodynamicConfiguration *configuration, HydrodynamicBoundaryCondition *boundaryCondition);
    virtual ~NonVerticallyIntegratedHydrodynamicBoundaryConditionDialog();
};

#endif // NON_VERTICALLY_INTEGRATED_HYDRODYNAMIC_BOUNDARY_CONDITION_DIALOG_H
