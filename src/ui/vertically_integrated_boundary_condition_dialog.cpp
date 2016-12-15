#include <ui/vertically_integrated_boundary_condition_dialog.h>
#include "ui_vertically_integrated_boundary_condition_dialog.h"

#include <ui/hydrodynamic_vtk_widget.h>
#include <ui/time_series_dialog.h>

#include <vtkCellData.h>

#include <QMessageBox>

VerticallyIntegratedBoundaryConditionDialog::VerticallyIntegratedBoundaryConditionDialog(HydrodynamicConfiguration *configuration, HydrodynamicBoundaryCondition *boundaryCondition) :
    HydrodynamicBoundaryConditionDialog(configuration, boundaryCondition),
    ui(new Ui::VerticallyIntegratedBoundaryConditionDialog)
{
    ui->setupUi(this);
    this->setupBaseUi();
    
    this->originalTimeSeriesList = this->currentBoundaryCondition->getTimeSeriesList();
    this->timeSeriesList = originalTimeSeriesList;
    
    if (!isNewBoundaryCondition) {
        bool isConstantSelected = boundaryCondition->getFunction() == BoundaryConditionFunction::CONSTANT;
        
        ui->edtName->setText(boundaryCondition->getName());
        ui->cbxType->blockSignals(true);
        ui->cbxType->setCurrentText(boundaryCondition->getTypeLabel());
        ui->cbxType->blockSignals(false);
        ui->lblElementLabel->setText(boundaryCondition->getObjectTypeLabel());
        ui->lblElementIds->setText(boundaryCondition->getObjectIdsStr());
        ui->rdoConstant->setChecked(isConstantSelected);
        ui->rdoTimeSeries->setChecked(!isConstantSelected);
        ui->edtConstant->setEnabled(isConstantSelected);
        
        if (boundaryCondition->getType() == BoundaryConditionType::NORMAL_DEPTH) {
            ui->lblConstant->setText("Friction slope");
            ui->rdoTimeSeries->setDisabled(true);
        } else {
            ui->lblConstant->setText("Value");
            ui->rdoTimeSeries->setEnabled(true);
        }
        
        if (isConstantSelected) {
            ui->edtConstant->setText(QString::number(boundaryCondition->getConstantValue()));
        } else {
            ui->btnTimeSeries->setEnabled(boundaryCondition->getFunction() == BoundaryConditionFunction::TIME_SERIES);
        }
        
        btnIndividualObjectPicker->setEnabled(ui->cbxType->currentText() == "Water Level");
    }
}

VerticallyIntegratedBoundaryConditionDialog::~VerticallyIntegratedBoundaryConditionDialog() {
    delete ui;
}

void VerticallyIntegratedBoundaryConditionDialog::on_cbxType_currentIndexChanged(const QString &type) {
    bool isWaterLevel = type == "Water Level";
    QString elementIds = "-";
    
    if (currentBoundaryCondition->getObjectIds().isEmpty()) {
        btnIndividualObjectPicker->setEnabled(isWaterLevel);
        btnIndividualObjectPicker->setChecked(false);
        btnMultipleObjectPicker->setEnabled(true);
        btnMultipleObjectPicker->setChecked(false);
    } else {
        QString question = tr("Changing the condition type will remove the selected objects. Are you sure?");
        QMessageBox::StandardButton button = QMessageBox::question(this, tr("Hydrodynamic Boundary Condition"), question);
        
        if (button == QMessageBox::No) {
            elementIds = currentBoundaryCondition->getObjectIdsStr();
            ui->cbxType->blockSignals(true);
            ui->cbxType->setCurrentText(type);
            ui->cbxType->blockSignals(false);
        } else {
            HydrodynamicVTKWidget *vtkWidget = hydrodynamicDataDialog->findChild<HydrodynamicVTKWidget*>("vtkWidget");
            
            currentBoundaryCondition->clearObjectIds();
            vtkWidget->getMouseInteractor()->renderBoundaryCondition(currentBoundaryCondition);
        }
    }
    
    ui->lblElementIds->setText(elementIds);
    ui->lblElementLabel->setText(isWaterLevel ? "Cells" : "Edges");
    
    if (!isWaterLevel) {
        bool isNormalDepth = type == "Normal Depth";
        
        ui->rdoConstant->setChecked(isNormalDepth);
        ui->rdoTimeSeries->setDisabled(isNormalDepth);
        ui->lblConstant->setText(isNormalDepth ? "Friction slope" : "Value");
    }
}

void VerticallyIntegratedBoundaryConditionDialog::on_btnTimeSeries_clicked() {
    TimeSeriesDialog *timeSeriesDialog = new TimeSeriesDialog(this, TimeSeriesType::DEFAULT);
    timeSeriesDialog->loadTimeSeriesList(&timeSeriesList);
    timeSeriesDialog->setObjectType(TimeSeriesObjectType::BOUNDARY_CONDITION);
    int exitCode = timeSeriesDialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        timeSeriesList = *timeSeriesDialog->getTimeSeriesList();
        currentBoundaryCondition->setTimeSeriesChanged(timeSeriesDialog->hasChanges());
    }
}

void VerticallyIntegratedBoundaryConditionDialog::accept() {
    if (!isValid()) {
        return;
    }
    
    BoundaryConditionType boundaryConditionType = BoundaryConditionType::WATER_LEVEL;
    
    if (ui->cbxType->currentText() == "Water Flow") {
        boundaryConditionType = BoundaryConditionType::WATER_FLOW;
    } else if (ui->cbxType->currentText() == "Normal Depth") {
        boundaryConditionType = BoundaryConditionType::NORMAL_DEPTH;
    }
    
    currentBoundaryCondition->setName(ui->edtName->text());
    currentBoundaryCondition->setType(boundaryConditionType);
    currentBoundaryCondition->setObjectIds(ui->lblElementIds->text());
    currentBoundaryCondition->setFunction(ui->rdoConstant->isChecked() ? BoundaryConditionFunction::CONSTANT : BoundaryConditionFunction::TIME_SERIES);
    currentBoundaryCondition->setConstantValue(ui->edtConstant->text().toDouble());
    currentBoundaryCondition->setInputModule(InputModule::HYDRODYNAMIC);
    currentBoundaryCondition->setVerticallyIntegrated(true);
    
    if (ui->rdoTimeSeries->isChecked()) {
        currentBoundaryCondition->setTimeSeriesList(timeSeriesList);
    }

    configuration->addBoundaryCondition(currentBoundaryCondition);
    emit boundaryConditionUpdated(currentBoundaryCondition);
    
    HydrodynamicBoundaryConditionDialog::accept();
}

void VerticallyIntegratedBoundaryConditionDialog::reject() {
    currentBoundaryCondition->setTimeSeriesList(originalTimeSeriesList);
    HydrodynamicBoundaryConditionDialog::reject();
}

void VerticallyIntegratedBoundaryConditionDialog::closeEvent(QCloseEvent *event) {
    if (!isNewBoundaryCondition) {
        currentBoundaryCondition->setTimeSeriesList(originalTimeSeriesList);
    }
    
    HydrodynamicBoundaryConditionDialog::closeEvent(event);
}

bool VerticallyIntegratedBoundaryConditionDialog::isValid() {
    if (ui->edtName->text().isEmpty()) {
        QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), tr("Please input the boundary condition name."));
        return false;
    }
    
    if (currentBoundaryCondition->getObjectIds().isEmpty()) {
        QString message = tr("Please pick at least one %1 from the grid.").arg(ui->cbxType->currentText() == "Water Level" ? "cell" : "edge");
        QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), message);
        return false;
    }
    
    if (ui->rdoConstant->isChecked() && ui->edtConstant->text().isEmpty()) {
        QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), tr("Please input a valid constant value."));
        return false;
    }
    
    if (ui->rdoTimeSeries->isChecked() && timeSeriesList.isEmpty()) {
        QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), tr("Please provide a valid time series list."));
        return false;
    }
    
    return true;
}
