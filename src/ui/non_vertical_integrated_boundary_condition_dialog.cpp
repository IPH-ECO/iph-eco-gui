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
        bool useVerticalIntegratedOutflow = boundaryCondition->useVerticalIntegratedOutflow();
        
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
        
        ui->chkVIO->setEnabled(boundaryCondition->getType() == BoundaryConditionType::WATER_FLOW);
        ui->chkVIO->setChecked(useVerticalIntegratedOutflow);
        ui->edtMinimumElevation->setEnabled(!useVerticalIntegratedOutflow);
        ui->edtMaximumElevation->setEnabled(!useVerticalIntegratedOutflow);
        if (!useVerticalIntegratedOutflow) {
            ui->edtMinimumElevation->setText(QString::number(boundaryCondition->getMinimumElevation()));
            ui->edtMaximumElevation->setText(QString::number(boundaryCondition->getMaximumElevation()));
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
        ui->chkVIO->setDisabled(isNormalDepth);
        ui->edtMinimumElevation->setDisabled(true);
        ui->edtMaximumElevation->setDisabled(true);
    } else {
        ui->chkVIO->setDisabled(true);
        ui->edtMinimumElevation->setDisabled(isWaterLevel || ui->chkVIO->isChecked());
        ui->edtMaximumElevation->setDisabled(isWaterLevel || ui->chkVIO->isChecked());
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
    
    if (ui->edtMinimumElevation->isEnabled()) {
        if (ui->edtMinimumElevation->text().isEmpty() || ui->edtMaximumElevation->text().isEmpty()) {
            QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), tr("Elevation interval can't be blank."));
            return false;
        } else if (ui->edtMinimumElevation->text().toDouble() > ui->edtMaximumElevation->text().toDouble()) {
            QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), tr("Invalid elevation interval."));
            return false;
        }
        
        GridDataConfiguration *gridDataConfiguration = configuration->getGridDataConfiguration();
        Mesh *mesh = gridDataConfiguration->getMesh();
        QSet<vtkIdType> boundaryCells = mesh->getBoundaryCellIds(currentBoundaryCondition->getVTKObjectIds());
        GridData *bathymetryGridData = nullptr;
        
        for (GridData *gridData : gridDataConfiguration->getGridDataVector()) {
            if (gridData->getGridDataType() == GridDataType::BATHYMETRY) {
                bathymetryGridData = gridData;
                break;
            }
        }
        
        double minimumElevation = ui->edtMinimumElevation->text().toDouble();
        double minimumWeight = std::numeric_limits<double>::min();
        
        for (vtkIdType cellId : boundaryCells) {
            QByteArray arrayName = bathymetryGridData->getName().toLocal8Bit();
            double weight = mesh->getMeshPolyData()->GetCellData()->GetScalars(arrayName.constData())->GetTuple1(cellId);
            
            if (minimumWeight == std::numeric_limits<double>::min()) {
                minimumWeight = weight;
            } else if (weight < minimumWeight) {
                minimumWeight = weight;
            }
        }
        
        if (minimumElevation < minimumWeight) {
            QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), tr("Minimum elevation must be greater than or equal to %1.").arg(minimumWeight));
            return false;
        }
    }
    
    return true;
}
