#include <ui/water_quality_boundary_condition_dialog.h>
#include "ui_water_quality_boundary_condition_dialog.h"

#include <ui/water_quality_dialog.h>
#include <ui/time_series_dialog.h>
#include "ui_water_quality_dialog.h"

#include <QMessageBox>

WaterQualityBoundaryConditionDialog::WaterQualityBoundaryConditionDialog(WaterQualityDialog *parent, WaterQualityConfiguration *configuration, WaterQualityBoundaryCondition *boundaryCondition) :
	QDialog(parent),
	ui(new Ui::WaterQualityBoundaryConditionDialog),
	currentConfiguration(configuration),
	currentBoundaryCondition(boundaryCondition),
	isNewBoundaryCondition(boundaryCondition == nullptr)
{
	ui->setupUi(this);
    
    for (HydrodynamicBoundaryCondition *boundaryCondition : configuration->getHydrodynamicConfiguration()->getBoundaryConditions()) {
        ui->cbxHydroBoundaryCondition->addItem(boundaryCondition->getName());
    }
    
    if (!isNewBoundaryCondition) {
        ui->cbxHydroBoundaryCondition->setCurrentText(currentBoundaryCondition->getName());
        ui->cbxVariable->setCurrentText(currentBoundaryCondition->getVariable());
    }
}

WaterQualityBoundaryConditionDialog::~WaterQualityBoundaryConditionDialog() {
	delete ui;
}

void WaterQualityBoundaryConditionDialog::on_btnTimeSeries_clicked() {
    TimeSeriesDialog *timeSeriesDialog = new TimeSeriesDialog(this, TimeSeriesType::DEFAULT);
    timeSeriesDialog->loadTimeSeriesList(&timeSeriesList);
    timeSeriesDialog->setBoundaryCondition(currentBoundaryCondition);
    int exitCode = timeSeriesDialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        timeSeriesList = *timeSeriesDialog->getTimeSeriesList();
        currentBoundaryCondition->setTimeSeriesChanged(timeSeriesDialog->hasChanges());
    }
}

void WaterQualityBoundaryConditionDialog::closeEvent(QCloseEvent *event) {
    undoChanges();
    QDialog::closeEvent(event);
}

void WaterQualityBoundaryConditionDialog::accept() {
    if (!isValid()) {
        return;
    }
    
    HydrodynamicConfiguration *hydrodynamicConfiguration = currentConfiguration->getHydrodynamicConfiguration();
    
    currentBoundaryCondition->setHydrodynamicBoundaryCondition(hydrodynamicConfiguration->getBoundaryCondition(ui->cbxHydroBoundaryCondition->currentText()));
    currentBoundaryCondition->setFunction(ui->rdoConstant->isChecked() ? BoundaryConditionFunction::CONSTANT : BoundaryConditionFunction::TIME_SERIES);
    currentBoundaryCondition->setConstantValue(ui->edtConstant->text().toDouble());
    currentBoundaryCondition->setInputModule(InputModule::WATER_QUALITY);
    
    if (ui->rdoTimeSeries->isChecked()) {
        currentBoundaryCondition->setTimeSeriesList(timeSeriesList);
    }
    
    currentConfiguration->addBoundaryCondition(currentBoundaryCondition);
    
    WaterQualityDialog *waterQualityDialog = static_cast<WaterQualityDialog*>(parentWidget());
    int row = -1;
    
    if (isNewBoundaryCondition) {
        row = waterQualityDialog->ui->tblBoundaryConditions->rowCount();
        waterQualityDialog->ui->tblBoundaryConditions->insertRow(row);
    } else {
        row = waterQualityDialog->ui->tblBoundaryConditions->currentRow();
    }
    
    waterQualityDialog->ui->tblBoundaryConditions->setItem(row, 0, new QTableWidgetItem(currentBoundaryCondition->getTypeLabel()));
    waterQualityDialog->ui->tblBoundaryConditions->setItem(row, 1, new QTableWidgetItem(currentBoundaryCondition->getFunctionLabel()));
    
    QDialog::accept();
}

void WaterQualityBoundaryConditionDialog::reject() {
    undoChanges();
    QDialog::reject();
}

bool WaterQualityBoundaryConditionDialog::isValid() {
    if (ui->cbxHydroBoundaryCondition->currentIndex() == - 1) {
        QMessageBox::warning(this, tr("Water Quality Boundary Condition"), tr("Hydrodynamic boundary condition can't be empty"));
        return false;
    }
    
    if (ui->cbxVariable->currentIndex() == -1) {
        QMessageBox::warning(this, tr("Water Quality Boundary Condition"), tr("Variable can't be empty"));
        return false;
    }
    
    if (ui->rdoConstant->isChecked() && ui->edtConstant->text().isEmpty()) {
        QMessageBox::warning(this, tr("Water Quality Boundary Condition"), tr("Constant value can't be empty"));
        return false;
    }
    
    return true;
}

void WaterQualityBoundaryConditionDialog::undoChanges() {
    if (!isNewBoundaryCondition) {
        currentBoundaryCondition->setTimeSeriesList(originalTimeSeriesList);
    }
}