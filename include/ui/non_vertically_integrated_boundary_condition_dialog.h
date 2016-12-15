#ifndef NON_VERTICALLY_INTEGRATED_BOUNDARY_CONDITION_DIALOG_H
#define NON_VERTICALLY_INTEGRATED_BOUNDARY_CONDITION_DIALOG_H

#include <QDialog>
#include "hydrodynamic_data_dialog.h"
#include "hydrodynamic_boundary_condition_dialog.h"

namespace Ui {
	class NonVerticallyIntegratedBoundaryConditionDialog;
}

class NonVerticallyIntegratedBoundaryConditionDialog : public HydrodynamicBoundaryConditionDialog {
    Q_OBJECT
private:
    Ui::NonVerticallyIntegratedBoundaryConditionDialog *ui;
    QSet<VerticallyIntegratedRange*> unsavedRanges;
    
    void accept();
    bool isValid();
private slots:
    void on_btnAddRange_clicked();
    void on_btnRemoveRange_clicked();
    void onCbxFunctionCurrentTextChanged(const QString &text);
    void onBtnTimeSeriesClicked();
public:
    explicit NonVerticallyIntegratedBoundaryConditionDialog(HydrodynamicConfiguration *configuration, HydrodynamicBoundaryCondition *boundaryCondition);
    virtual ~NonVerticallyIntegratedBoundaryConditionDialog();
};

#endif // NON_VERTICALLY_INTEGRATED_BOUNDARY_CONDITION_DIALOG_H
