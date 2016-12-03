#ifndef HYDRODYNAMIC_BOUNDARY_CONDITION_DIALOG_H
#define HYDRODYNAMIC_BOUNDARY_CONDITION_DIALOG_H

#include <QCloseEvent>
#include <QToolButton>
#include <QDialog>
#include <QWidget>
#include <QDialogButtonBox>
#include <QToolButton>

#include "hydrodynamic_data_dialog.h"

class HydrodynamicDataDialog;

class HydrodynamicBoundaryConditionDialog : public QDialog {
	Q_OBJECT
protected:
    HydrodynamicDataDialog *hydrodynamicDataDialog;
    HydrodynamicConfiguration *configuration;
    HydrodynamicBoundaryCondition *currentBoundaryCondition;
    QToolButton *btnIndividualObjectPicker;
    QToolButton *btnMultipleObjectPicker;
    QToolButton *btnClearSelection;
    QSet<vtkIdType> originalObjectIds;
    bool isNewBoundaryCondition;
    
    void setupUi();
    virtual void closeEvent(QCloseEvent *event);
    virtual void reject();
    virtual bool isValid() = 0;
public:
	explicit HydrodynamicBoundaryConditionDialog(HydrodynamicConfiguration *configuration, HydrodynamicBoundaryCondition *boundaryCondition);
    ~HydrodynamicBoundaryConditionDialog();
public slots:
    void btnIndividualObjectPicker_clicked(bool checked);
    void btnMultipleObjectPicker_clicked(bool checked);
private slots:
    void on_btnCellColor_clicked();
    void btnClearSelection_clicked();
    void showObjectIds();
    void toggleLabelsActor(bool show);
signals:
    void updateBoundaryCondition(HydrodynamicBoundaryCondition *boundaryCondition);
    void togglePicker(bool checked);
};

#endif // HYDRODYNAMIC_BOUNDARY_CONDITION_DIALOG_H
