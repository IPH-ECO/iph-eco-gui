#ifndef VERTICALLY_INTEGRATED_WATER_QUALITY_BOUNDARY_CONDITION_DIALOG_H
#define VERTICALLY_INTEGRATED_WATER_QUALITY_BOUNDARY_CONDITION_DIALOG_H

#include <domain/water_quality_boundary_condition.h>
#include <ui/water_quality_dialog.h>

#include <QCloseEvent>
#include <QDialog>
#include <QWidget>

namespace Ui {
	class VerticallyIntegratedWaterQualityBoundaryConditionDialog;
}

class VerticallyIntegratedWaterQualityBoundaryConditionDialog : public QDialog {
	Q_OBJECT
private:
    Ui::VerticallyIntegratedWaterQualityBoundaryConditionDialog *ui;
    WaterQualityConfiguration *currentConfiguration;
    WaterQualityBoundaryCondition *currentBoundaryCondition;
    QList<TimeSeries*> originalTimeSeriesList;
    QList<TimeSeries*> timeSeriesList;
    bool isNewBoundaryCondition;
    
    void closeEvent(QCloseEvent *event);
    virtual void accept();
    virtual void reject();
    bool isValid();
    void undoChanges();
    
public:
	explicit VerticallyIntegratedWaterQualityBoundaryConditionDialog(WaterQualityDialog *parent, WaterQualityConfiguration *configuration, WaterQualityBoundaryCondition *boundaryCondition);
    ~VerticallyIntegratedWaterQualityBoundaryConditionDialog();
    
private slots:
    void on_btnTimeSeries_clicked();
};

#endif // VERTICALLY_INTEGRATED_WATER_QUALITY_BOUNDARY_CONDITION_DIALOG_H
