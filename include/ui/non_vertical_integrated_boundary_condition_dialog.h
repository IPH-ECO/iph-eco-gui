#ifndef NON_VERTICAL_INTEGRATED_BOUNDARY_CONDITION_DIALOG_H
#define NON_VERTICAL_INTEGRATED_BOUNDARY_CONDITION_DIALOG_H

#include <QDialog>
#include "hydrodynamic_data_dialog.h"
#include "hydrodynamic_boundary_condition_dialog.h"

namespace Ui {
	class NonVerticalIntegratedBoundaryConditionDialog;
}

class NonVerticalIntegratedBoundaryConditionDialog : public HydrodynamicBoundaryConditionDialog {
    Q_OBJECT
private:
    Ui::NonVerticalIntegratedBoundaryConditionDialog *ui;
    QList<TimeSeries*> originalTimeSeriesList;
    QList<TimeSeries*> timeSeriesList;
    
    void accept();
    virtual void reject();
    virtual void closeEvent(QCloseEvent *event);
    bool isValid();
public:
    explicit NonVerticalIntegratedBoundaryConditionDialog(HydrodynamicConfiguration *configuration, HydrodynamicBoundaryCondition *boundaryCondition);
    virtual ~NonVerticalIntegratedBoundaryConditionDialog();
private slots:
	void on_cbxType_currentIndexChanged(const QString &type);
    void on_btnTimeSeries_clicked();
};

#endif // NON_VERTICAL_INTEGRATED_BOUNDARY_CONDITION_DIALOG_H
