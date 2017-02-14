#include <ui/vertically_integrated_water_quality_boundary_condition_dialog.h>
#include "ui_vertically_integrated_water_quality_boundary_condition_dialog.h"

#include <ui/water_quality_dialog.h>
#include <ui/time_series_dialog.h>
#include <repository/water_quality_repository.h>
#include "ui_water_quality_dialog.h"

#include <QMessageBox>

VerticallyIntegratedWaterQualityBoundaryConditionDialog::VerticallyIntegratedWaterQualityBoundaryConditionDialog(WaterQualityDialog *parent, WaterQualityConfiguration *configuration, WaterQualityBoundaryCondition *boundaryCondition) :
	QDialog(parent),
	ui(new Ui::VerticallyIntegratedWaterQualityBoundaryConditionDialog),
	currentConfiguration(configuration),
	currentBoundaryCondition(boundaryCondition),
	isNewBoundaryCondition(boundaryCondition == nullptr)
{
	ui->setupUi(this);
    
    WaterQualityRepository *repository = WaterQualityRepository::getInstance();
    
    for (HydrodynamicBoundaryCondition *boundaryCondition : configuration->getHydrodynamicConfiguration()->getBoundaryConditions()) {
        ui->cbxHydroBoundaryCondition->addItem(boundaryCondition->getName());
    }
    
    for (WaterQualityParameter *variable : repository->getBoundaryConditionVariables()) {
        WaterQualityParameter *target = variable->getTarget();
        
        if (target) {
            if (target->getItemWidget()->checkState(0) == Qt::Checked) {
                if (variable->isGroup()) {
                    QString groupParameterName = variable->getName() + "Groups";
                    WaterQualityParameter *groupParameter = currentConfiguration->getParameter(groupParameterName, WaterQualityParameterSection::PARAMETER);
                    
                    for (WaterQualityParameter *group : groupParameter->getChildren()) {
                        for (int j = 1; j <= group->getValue(); j++) {
                            ui->cbxVariable->addItem(QString("%1 - %2 %3").arg(variable->getLabel()).arg(group->getLabel()).arg(j));
                        }
                    }
                } else if (variable->isRadio()) {
                    ui->cbxVariable->addItem(variable->getLabel(), qVariantFromValue((void*) variable));
                } else {
                    ui->cbxVariable->addItem(variable->getLabel());
                }
            }
        } else {
            ui->cbxVariable->addItem(variable->getLabel());
        }
    }
    
    int count = ui->cbxVariable->count();
    
    for (int i = 0; i < count; i++) {
        WaterQualityParameter *variable = static_cast<WaterQualityParameter*>(ui->cbxVariable->itemData(i).value<void*>());
        
        if (!variable) {
            continue;
        }
        
        WaterQualityParameter *targetParameter = variable->getTarget();
        
        if (targetParameter->isChecked()) {
            for (WaterQualityParameter *child : targetParameter->getChildren()) {
                if (child->isChecked()) {
                    ui->cbxVariable->removeItem(i);
                    count--;
                    break;
                }
            }
        }
    }
    
    if (isNewBoundaryCondition) {
        currentBoundaryCondition = new WaterQualityBoundaryCondition();
    }
    
    bool isConstant = currentBoundaryCondition->getFunction() == BoundaryConditionFunction::CONSTANT;
    
    ui->cbxHydroBoundaryCondition->setCurrentText(currentBoundaryCondition->getName());
    ui->cbxVariable->setCurrentText(currentBoundaryCondition->getName());
    ui->rdoConstant->setChecked(isConstant);
    ui->edtConstant->setEnabled(isConstant);
    ui->rdoTimeSeries->setChecked(!isConstant);
    ui->btnTimeSeries->setEnabled(!isConstant);
    
    if (isConstant && !isNewBoundaryCondition) {
        ui->edtConstant->setText(QString::number(currentBoundaryCondition->getConstantValue()));
    }
    
    this->originalTimeSeriesList = currentBoundaryCondition->getTimeSeriesList();
    this->timeSeriesList = originalTimeSeriesList;
}

VerticallyIntegratedWaterQualityBoundaryConditionDialog::~VerticallyIntegratedWaterQualityBoundaryConditionDialog() {
	delete ui;
}

void VerticallyIntegratedWaterQualityBoundaryConditionDialog::on_btnTimeSeries_clicked() {
    TimeSeriesDialog *timeSeriesDialog = new TimeSeriesDialog(this, TimeSeriesType::DEFAULT);
    timeSeriesDialog->loadTimeSeriesList(&timeSeriesList);
    timeSeriesDialog->setObjectType(TimeSeriesObjectType::BOUNDARY_CONDITION);
    int exitCode = timeSeriesDialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        timeSeriesList = *timeSeriesDialog->getTimeSeriesList();
        currentBoundaryCondition->setTimeSeriesChanged(timeSeriesDialog->hasChanges());
    }
}

void VerticallyIntegratedWaterQualityBoundaryConditionDialog::closeEvent(QCloseEvent *event) {
    undoChanges();
    QDialog::closeEvent(event);
}

void VerticallyIntegratedWaterQualityBoundaryConditionDialog::accept() {
    if (!isValid()) {
        return;
    }
    
    HydrodynamicConfiguration *hydrodynamicConfiguration = currentConfiguration->getHydrodynamicConfiguration();
    HydrodynamicBoundaryCondition *hydrodynamicBoundaryCondition = hydrodynamicConfiguration->getBoundaryCondition(ui->cbxHydroBoundaryCondition->currentText());
    
    currentBoundaryCondition->setName(ui->cbxVariable->currentText());
    currentBoundaryCondition->setHydrodynamicBoundaryCondition(hydrodynamicBoundaryCondition);
    currentBoundaryCondition->setFunction(ui->rdoConstant->isChecked() ? BoundaryConditionFunction::CONSTANT : BoundaryConditionFunction::TIME_SERIES);
    currentBoundaryCondition->setConstantValue(ui->edtConstant->text().toDouble());
    currentBoundaryCondition->setInputModule(InputModule::WATER_QUALITY);
    currentBoundaryCondition->setVerticallyIntegrated(true);
    
    if (ui->rdoTimeSeries->isChecked()) {
        currentBoundaryCondition->setTimeSeriesList(timeSeriesList);
    }
    
    currentConfiguration->addBoundaryCondition(currentBoundaryCondition);
    emit boundaryConditionUpdated(currentBoundaryCondition);
    
    QDialog::accept();
}

void VerticallyIntegratedWaterQualityBoundaryConditionDialog::reject() {
    undoChanges();
    QDialog::reject();
}

bool VerticallyIntegratedWaterQualityBoundaryConditionDialog::isValid() {
    if (ui->cbxHydroBoundaryCondition->currentIndex() == - 1) {
        QMessageBox::warning(this, tr("Water Quality Boundary Condition"), tr("Hydrodynamic boundary condition can't be empty"));
        return false;
    }
    
    if (ui->cbxVariable->currentIndex() == -1) {
        QMessageBox::warning(this, tr("Water Quality Boundary Condition"), tr("Variable can't be empty"));
        return false;
    }
    
    HydrodynamicConfiguration *hydrodynamicConfiguration = currentConfiguration->getHydrodynamicConfiguration();
    HydrodynamicBoundaryCondition *hydrodynamicBoundaryCondition = hydrodynamicConfiguration->getBoundaryCondition(ui->cbxHydroBoundaryCondition->currentText());
    
    for (WaterQualityBoundaryCondition *existentBoundaryCondition : currentConfiguration->getBoundaryConditions()) {
        if (existentBoundaryCondition->getHydrodynamicBoundaryCondition() == hydrodynamicBoundaryCondition && existentBoundaryCondition->getName() == ui->cbxVariable->currentText() && currentBoundaryCondition != existentBoundaryCondition) {
            QMessageBox::warning(this, tr("Water Quality Boundary Condition"), QString("A boundary condition using variable %1 already exists.").arg(ui->cbxVariable->currentText()));
            return false;
        }
    }
    
    if (ui->rdoConstant->isChecked() && ui->edtConstant->text().isEmpty()) {
        QMessageBox::warning(this, tr("Water Quality Boundary Condition"), tr("Constant value can't be empty"));
        return false;
    }
    
    return true;
}

void VerticallyIntegratedWaterQualityBoundaryConditionDialog::undoChanges() {
    if (!isNewBoundaryCondition) {
        currentBoundaryCondition->setTimeSeriesList(originalTimeSeriesList);
    }
}
