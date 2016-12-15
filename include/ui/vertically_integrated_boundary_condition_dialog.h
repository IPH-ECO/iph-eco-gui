#ifndef VERTICALLY_INTEGRATED_BOUNDARY_CONDITION_DIALOG_H
#define VERTICALLY_INTEGRATED_BOUNDARY_CONDITION_DIALOG_H

#include <QDialog>
#include "hydrodynamic_data_dialog.h"
#include "hydrodynamic_boundary_condition_dialog.h"

namespace Ui {
	class VerticallyIntegratedBoundaryConditionDialog;
}

class VerticallyIntegratedBoundaryConditionDialog : public HydrodynamicBoundaryConditionDialog {
    Q_OBJECT
private:
    Ui::VerticallyIntegratedBoundaryConditionDialog *ui;
    QList<TimeSeries*> originalTimeSeriesList;
    QList<TimeSeries*> timeSeriesList;
    
    void accept();
    virtual void reject();
    virtual void closeEvent(QCloseEvent *event);
    bool isValid();
public:
    explicit VerticallyIntegratedBoundaryConditionDialog(HydrodynamicConfiguration *configuration, HydrodynamicBoundaryCondition *boundaryCondition);
    virtual ~VerticallyIntegratedBoundaryConditionDialog();
private slots:
	void on_cbxType_currentIndexChanged(const QString &type);
    void on_btnTimeSeries_clicked();
};

#endif // VERTICALLY_INTEGRATED_BOUNDARY_CONDITION_DIALOG_H
