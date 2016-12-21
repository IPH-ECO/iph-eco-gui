#ifndef WATER_QUALITY_BOUNDARY_CONDITION_DIALOG_H
#define WATER_QUALITY_BOUNDARY_CONDITION_DIALOG_H

#include <domain/water_quality_configuration.h>

#include <QCloseEvent>
#include <QDialog>
#include <QWidget>

class WaterQualityBoundaryConditionDialog : public QDialog {
	Q_OBJECT
protected:
    WaterQualityConfiguration *configuration;
    WaterQualityBoundaryCondition *currentBoundaryCondition;
    bool isNewBoundaryCondition;
    
    virtual bool isValid() = 0;
public:
	explicit WaterQualityBoundaryConditionDialog(WaterQualityConfiguration *configuration, WaterQualityBoundaryCondition *boundaryCondition);
    ~WaterQualityBoundaryConditionDialog();
signals:
    void boundaryConditionUpdated(WaterQualityBoundaryCondition *boundaryCondition);
};

#endif // WATER_QUALITY_BOUNDARY_CONDITION_DIALOG_H
