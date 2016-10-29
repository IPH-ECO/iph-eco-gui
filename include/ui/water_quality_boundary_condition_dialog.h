#ifndef WATER_QUALITY_BOUNDARY_CONDITION_DIALOG_H
#define WATER_QUALITY_BOUNDARY_CONDITION_DIALOG_H

#include <domain/water_quality_boundary_condition.h>
#include <ui/water_quality_dialog.h>

#include <QCloseEvent>
#include <QDialog>
#include <QWidget>

namespace Ui {
	class WaterQualityBoundaryConditionDialog;
}

class WaterQualityBoundaryConditionDialog : public QDialog {
	Q_OBJECT
private:
    Ui::WaterQualityBoundaryConditionDialog *ui;
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
	explicit WaterQualityBoundaryConditionDialog(WaterQualityDialog *parent, WaterQualityConfiguration *configuration, WaterQualityBoundaryCondition *boundaryCondition);
    ~WaterQualityBoundaryConditionDialog();
    
private slots:
    void on_btnTimeSeries_clicked();
};

#endif // WATER_QUALITY_BOUNDARY_CONDITION_DIALOG_H