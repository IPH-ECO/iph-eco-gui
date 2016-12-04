#include <ui/non_vertical_integrated_boundary_condition_dialog.h>
#include "ui_non_vertical_integrated_boundary_condition_dialog.h"

#include <ui/hydrodynamic_vtk_widget.h>

#include <vtkCellData.h>

#include <QMessageBox>

NonVerticalIntegratedBoundaryConditionDialog::NonVerticalIntegratedBoundaryConditionDialog(HydrodynamicConfiguration *configuration, HydrodynamicBoundaryCondition *boundaryCondition) :
    HydrodynamicBoundaryConditionDialog(configuration, boundaryCondition),
    ui(new Ui::NonVerticalIntegratedBoundaryConditionDialog)
{
    ui->setupUi(this);
    this->setupUi();
    
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

NonVerticalIntegratedBoundaryConditionDialog::~NonVerticalIntegratedBoundaryConditionDialog() {
    delete ui;
}

void NonVerticalIntegratedBoundaryConditionDialog::on_cbxType_currentIndexChanged(const QString &type) {
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

void NonVerticalIntegratedBoundaryConditionDialog::on_btnTimeSeries_clicked() {
    
}

void NonVerticalIntegratedBoundaryConditionDialog::accept() {
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
    currentBoundaryCondition->setInputModule(InputModule::HYDRODYNAMIC);
    
    configuration->addBoundaryCondition(currentBoundaryCondition);
    
    HydrodynamicVTKWidget *vtkWidget = hydrodynamicDataDialog->findChild<HydrodynamicVTKWidget*>("vtkWidget");
    QTableWidget *tblBoundaryConditions = hydrodynamicDataDialog->findChild<QTableWidget*>("tblBoundaryConditions");
    int row = -1;
    
    if (isNewBoundaryCondition) {
        QTableWidgetItem *item = new QTableWidgetItem();
        
        item->setData(Qt::UserRole, qVariantFromValue((void*) currentBoundaryCondition));
        row = tblBoundaryConditions->rowCount();
        tblBoundaryConditions->insertRow(row);
        tblBoundaryConditions->setVerticalHeaderItem(row, item);
    } else {
        row = tblBoundaryConditions->currentRow();
        vtkWidget->getMouseInteractor()->highlightBoundaryCondition(currentBoundaryCondition, true);
    }
    
    tblBoundaryConditions->setItem(row, 0, new QTableWidgetItem(currentBoundaryCondition->getName()));
    tblBoundaryConditions->setItem(row, 1, new QTableWidgetItem(currentBoundaryCondition->getTypeLabel()));
    tblBoundaryConditions->setItem(row, 2, new QTableWidgetItem(currentBoundaryCondition->getFunctionLabel()));
    
    btnIndividualObjectPicker_clicked(false);
    btnMultipleObjectPicker_clicked(false);
    hydrodynamicDataDialog->toggleZoomAreaAction(true);
    hydrodynamicDataDialog->toggleWidgets(true);
    
    QDialog::accept();
    
    HydrodynamicBoundaryConditionDialog::accept();
}

void NonVerticalIntegratedBoundaryConditionDialog::reject() {
    currentBoundaryCondition->setTimeSeriesList(originalTimeSeriesList);
    HydrodynamicBoundaryConditionDialog::reject();
}

void NonVerticalIntegratedBoundaryConditionDialog::closeEvent(QCloseEvent *event) {
    if (!isNewBoundaryCondition) {
        currentBoundaryCondition->setTimeSeriesList(originalTimeSeriesList);
    }
    
    HydrodynamicBoundaryConditionDialog::closeEvent(event);
}

bool NonVerticalIntegratedBoundaryConditionDialog::isValid() {
    if (ui->edtName->text().isEmpty()) {
        QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), tr("Please input Hydrodynamic Boundary Condition name."));
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
