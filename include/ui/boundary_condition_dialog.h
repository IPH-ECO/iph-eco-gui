#ifndef BOUNDARY_CONDITION_DIALOG_H
#define BOUNDARY_CONDITION_DIALOG_H

#include <QCloseEvent>
#include <QToolButton>
#include <QDialog>
#include <QWidget>

#include "hydrodynamic_data_dialog.h"

namespace Ui {
	class BoundaryConditionDialog;
}

class HydrodynamicDataDialog;

class BoundaryConditionDialog : public QDialog {
	Q_OBJECT
    
    friend class HydrodynamicDataDialog;
private:
	Ui::BoundaryConditionDialog *ui;
    HydrodynamicConfiguration *configuration;
    BoundaryCondition *currentBoundaryCondition;
    QToolButton *btnIndividualObjectPicker;
    QToolButton *btnMultipleCellPicker;
    HydrodynamicDataDialog *hydrodynamicDataDialog;
    QSet<vtkIdType> originalObjectIds;
    bool isNewBoundaryCondition;
    
    void closeEvent(QCloseEvent *event);
    virtual void accept();
    virtual void reject();
    bool isValid();
    void undoChanges();
public:
	explicit BoundaryConditionDialog(HydrodynamicConfiguration *configuration, BoundaryCondition *boundaryCondition);
    ~BoundaryConditionDialog();
    
    void setHydrodynamicDataDialog(HydrodynamicDataDialog *dialog);
private slots:
    void on_rdoWaterLevel_clicked(bool checked);
    void on_rdoWaterFlow_clicked(bool checked);
    void on_btnTimeSeries_clicked();
    void on_btnCellColor_clicked();
    void btnIndividualObjectPicker_clicked(bool checked);
    void btnMultipleCellPicker_clicked(bool checked);
    void btnClearSelection_clicked();
    void showObjectIds();
    void toggleLabelsActor(bool show);
};

#endif // BOUNDARY_CONDITION_DIALOG_H