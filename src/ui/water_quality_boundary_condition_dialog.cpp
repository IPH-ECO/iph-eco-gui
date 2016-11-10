#include <ui/water_quality_boundary_condition_dialog.h>
#include "ui_water_quality_boundary_condition_dialog.h"

#include <ui/water_quality_dialog.h>
#include <ui/time_series_dialog.h>
#include <repository/water_quality_repository.h>
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
    
    WaterQualityRepository *repository = WaterQualityRepository::getInstance();
    
    for (HydrodynamicBoundaryCondition *boundaryCondition : configuration->getHydrodynamicConfiguration()->getBoundaryConditions()) {
        ui->cbxHydroBoundaryCondition->addItem(boundaryCondition->getName());
    }
    
    QList<WaterQualityParameter*> variables = repository->getBoundaryConditionVariables();
    
    for (WaterQualityParameter *variable : variables) {
        if (variable->getTarget()) {
            if (variable->isGroup()) {
                WaterQualityParameter *structureTarget = currentConfiguration->getParameter(variable->getName(), WaterQualityParameterSection::STRUCTURE);
                
                if (structureTarget->isChecked()) {
                    QList<WaterQualityParameter*> groups = variable->getTarget()->getChildren();
                    
                    for (WaterQualityParameter *groupParameter : groups) {
                        for (int i = 1; i <= groupParameter->getValue(); i++) {
                            ui->cbxVariable->addItem(QString("%1 - %2 %3").arg(variable->getLabel()).arg(groupParameter->getLabel()).arg(i));
                        }
                    }
                }
            } else if (!variable->getTarget()->getItemWidget()->isHidden()) {
                ui->cbxVariable->addItem(variable->getLabel());
            }
        } else {
            ui->cbxVariable->addItem(variable->getLabel());
        }
    }
    
    if (!isNewBoundaryCondition) {
        bool isConstant = currentBoundaryCondition->getFunction() == BoundaryConditionFunction::CONSTANT;
        
        ui->cbxHydroBoundaryCondition->setCurrentText(currentBoundaryCondition->getName());
        ui->cbxVariable->setCurrentText(currentBoundaryCondition->getVariable());
        ui->rdoConstant->setChecked(isConstant);
        ui->rdoTimeSeries->setChecked(!isConstant);
        ui->btnTimeSeries->setEnabled(!isConstant);
        
        if (isConstant) {
            ui->edtConstant->setText(QString::number(currentBoundaryCondition->getConstantValue()));
        }
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
    
    if (isNewBoundaryCondition) {
        currentBoundaryCondition = new WaterQualityBoundaryCondition();
    }
    
    currentBoundaryCondition->setHydrodynamicBoundaryCondition(hydrodynamicConfiguration->getBoundaryCondition(ui->cbxHydroBoundaryCondition->currentText()));
    currentBoundaryCondition->setVariable(ui->cbxVariable->currentText());
    currentBoundaryCondition->setFunction(ui->rdoConstant->isChecked() ? BoundaryConditionFunction::CONSTANT : BoundaryConditionFunction::TIME_SERIES);
    currentBoundaryCondition->setConstantValue(ui->edtConstant->text().toDouble());
    currentBoundaryCondition->setInputModule(InputModule::WATER_QUALITY);
    
    if (ui->rdoTimeSeries->isChecked()) {
        currentBoundaryCondition->setTimeSeriesList(timeSeriesList);
    }
    
    currentConfiguration->addBoundaryCondition(currentBoundaryCondition);
    
    WaterQualityDialog *waterQualityDialog = static_cast<WaterQualityDialog*>(parentWidget());
    QTableWidget *tableWidget = waterQualityDialog->ui->tblBoundaryConditions;
    int row = -1;
    
    if (isNewBoundaryCondition) {
        QTableWidgetItem *headerItem = new QTableWidgetItem();
        
        row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        headerItem->setData(Qt::UserRole, qVariantFromValue((void*) currentBoundaryCondition));
        tableWidget->setVerticalHeaderItem(row, headerItem);
    } else {
        row = tableWidget->currentRow();
    }
    
    tableWidget->setItem(row, 0, new QTableWidgetItem(currentBoundaryCondition->getHydrodynamicBoundaryCondition()->getName()));
    tableWidget->setItem(row, 1, new QTableWidgetItem(currentBoundaryCondition->getVariable()));
    tableWidget->setItem(row, 2, new QTableWidgetItem(currentBoundaryCondition->getFunctionLabel()));
    
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
