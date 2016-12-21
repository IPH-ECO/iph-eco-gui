#ifndef NON_VERTICALLY_INTEGRATED_WATER_QUALITY_BOUNDARY_CONDITION_DIALOG_H
#define NON_VERTICALLY_INTEGRATED_WATER_QUALITY_BOUNDARY_CONDITION_DIALOG_H

#include <domain/water_quality_boundary_condition.h>
#include <ui/water_quality_dialog.h>

#include <QCloseEvent>
#include <QDialog>
#include <QWidget>

namespace Ui {
	class NonVerticallyIntegratedWaterQualityBoundaryConditionDialog;
}

class NonVerticallyIntegratedWaterQualityBoundaryConditionDialog : public QDialog {
	Q_OBJECT
private:
    Ui::NonVerticallyIntegratedWaterQualityBoundaryConditionDialog *ui;
    WaterQualityConfiguration *currentConfiguration;
    WaterQualityBoundaryCondition *currentBoundaryCondition;
    QSet<NonVerticallyIntegratedRange*> unsavedRanges;
    bool isNewBoundaryCondition;
    
    virtual void accept();
    virtual void reject();
    bool isValid();
    void undoChanges();
    
public:
	explicit NonVerticallyIntegratedWaterQualityBoundaryConditionDialog(QDialog *parent, WaterQualityConfiguration *configuration, WaterQualityBoundaryCondition *boundaryCondition);
    ~NonVerticallyIntegratedWaterQualityBoundaryConditionDialog();
    
private slots:
    void on_btnAddRange_clicked();
    void on_btnRemoveRange_clicked();
    void onCbxFunctionCurrentTextChanged(const QString &text);
    void onBtnTimeSeriesClicked();
    
signals:
    void boundaryConditionUpdated(WaterQualityBoundaryCondition *boundaryCondition);
};

#endif // NON_VERTICALLY_INTEGRATED_WATER_QUALITY_BOUNDARY_CONDITION_DIALOG_H
