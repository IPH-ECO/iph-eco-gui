#include <ui/water_quality_boundary_condition_dialog.h>

#include <QIcon>
#include <QLabel>
#include <QComboBox>
#include <QStringList>
#include <QMessageBox>
#include <QColorDialog>
#include <QApplication>

#include <ui/time_series_dialog.h>

WaterQualityBoundaryConditionDialog::WaterQualityBoundaryConditionDialog(WaterQualityConfiguration *configuration, WaterQualityBoundaryCondition *boundaryCondition) :
    configuration(configuration),
    currentBoundaryCondition(boundaryCondition),
    isNewBoundaryCondition(boundaryCondition == nullptr)
{
    if (isNewBoundaryCondition) {
        this->currentBoundaryCondition = new WaterQualityBoundaryCondition();
    }
}

WaterQualityBoundaryConditionDialog::~WaterQualityBoundaryConditionDialog() {
    if (isNewBoundaryCondition) {
        delete currentBoundaryCondition;
    }
}
