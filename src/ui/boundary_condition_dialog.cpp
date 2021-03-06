#include <ui/boundary_condition_dialog.h>
#include "ui_boundary_condition_dialog.h"
#include "ui_hydrodynamic_data_dialog.h"

#include <QIcon>
#include <vtkCell.h>
#include <QStringList>
#include <QMessageBox>
#include <QColorDialog>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkCellData.h>

#include <ui/hydrodynamic_vtk_widget.h>
#include <ui/time_series_dialog.h>

BoundaryConditionDialog::BoundaryConditionDialog(HydrodynamicConfiguration *configuration, BoundaryCondition *boundaryCondition) :
    QDialog(nullptr), ui(new Ui::BoundaryConditionDialog),
    configuration(configuration), currentBoundaryCondition(boundaryCondition), isNewBoundaryCondition(boundaryCondition == nullptr)
{
	ui->setupUi(this);

	btnIndividualObjectPicker = new QToolButton(this);
    btnIndividualObjectPicker->setCheckable(true);
	btnIndividualObjectPicker->setIcon(QIcon(":/icons/individual-object-picker.png"));
	btnIndividualObjectPicker->setToolTip("Individual cell picker");
	ui->buttonBox->addButton(btnIndividualObjectPicker, QDialogButtonBox::ActionRole);
    connect(btnIndividualObjectPicker, SIGNAL(clicked(bool)), this, SLOT(btnIndividualObjectPicker_clicked(bool)));

	btnMultipleObjectPicker = new QToolButton(this);
    btnMultipleObjectPicker->setCheckable(true);
	btnMultipleObjectPicker->setIcon(QIcon(":/icons/multiple-object-picker.png"));
	btnMultipleObjectPicker->setToolTip("Multiple object picker");
	ui->buttonBox->addButton(btnMultipleObjectPicker, QDialogButtonBox::ActionRole);
    ui->buttonBox->setCenterButtons(true);
    connect(btnMultipleObjectPicker, SIGNAL(clicked(bool)), this, SLOT(btnMultipleObjectPicker_clicked(bool)));
    
    QToolButton *btnClearSelection = new QToolButton(this);
    btnClearSelection->setIcon(QIcon(":/icons/edit-clear-2.png"));
    btnClearSelection->setToolTip("Clear selection");
    ui->buttonBox->addButton(btnClearSelection, QDialogButtonBox::ActionRole);
    connect(btnClearSelection, SIGNAL(clicked()), this, SLOT(btnClearSelection_clicked()));
    
    if (currentBoundaryCondition) {
        bool isConstantSelected = boundaryCondition->getFunction() == BoundaryConditionFunction::CONSTANT;
        bool useVerticalIntegratedOutflow = boundaryCondition->useVerticalIntegratedOutflow();
        
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
    } else {
        this->currentBoundaryCondition = new BoundaryCondition();
    }
    
    QColor color = QColor(this->currentBoundaryCondition->getCellColor());
    QPixmap px(10, 10);
    
    px.fill(color);
    ui->btnCellColor->setIcon(px);
    
    this->originalObjectIds = this->currentBoundaryCondition->getObjectIds();
    this->originalTimeSeriesList = this->currentBoundaryCondition->getTimeSeriesList();
    this->timeSeriesList = originalTimeSeriesList;
}

BoundaryConditionDialog::~BoundaryConditionDialog() {
    if (isNewBoundaryCondition) {
        delete currentBoundaryCondition;
    }
    
    delete ui;
}

void BoundaryConditionDialog::setHydrodynamicDataDialog(HydrodynamicDataDialog *dialog) {
    this->hydrodynamicDataDialog = dialog;
    
    this->hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor()->setBoundaryCondition(currentBoundaryCondition);
    this->hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor()->highlightBoundaryCondition(currentBoundaryCondition, false);
    this->currentBoundaryCondition->getLabelsActor()->SetVisibility(hydrodynamicDataDialog->isCellLabelsActionChecked());
    
    connect(hydrodynamicDataDialog->toggleCellLabelsAction, SIGNAL(triggered(bool)), this, SLOT(toggleLabelsActor(bool)));
    connect(currentBoundaryCondition, SIGNAL(objectIdsChanged()), this, SLOT(showObjectIds()));
}

void BoundaryConditionDialog::on_cbxType_currentIndexChanged(const QString &type) {
    bool isWaterLevel = type == "Water Level";
    QString elementIds = "-";
    
    if (currentBoundaryCondition->getObjectIds().isEmpty()) {
        btnIndividualObjectPicker->setEnabled(isWaterLevel);
        btnIndividualObjectPicker->setChecked(false);
        btnMultipleObjectPicker->setEnabled(true);
        btnMultipleObjectPicker->setChecked(false);
    } else {
        QString question = tr("Changing the condition type will remove the selected objects. Are you sure?");
        QMessageBox::StandardButton button = QMessageBox::question(this, tr("Boundary Condition"), question);
        
        if (button == QMessageBox::No) {
            elementIds = currentBoundaryCondition->getObjectIdsStr();
            ui->cbxType->blockSignals(true);
            ui->cbxType->setCurrentText(type);
            ui->cbxType->blockSignals(false);
        } else {
            currentBoundaryCondition->clearObjectIds();
            hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor()->renderBoundaryCondition(currentBoundaryCondition);
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

void BoundaryConditionDialog::on_btnTimeSeries_clicked() {
    TimeSeriesDialog *timeSeriesDialog = new TimeSeriesDialog(this, TimeSeriesType::DEFAULT);
    timeSeriesDialog->loadTimeSeriesList(&timeSeriesList);
    timeSeriesDialog->setBoundaryCondition(currentBoundaryCondition);
    int exitCode = timeSeriesDialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        timeSeriesList = *timeSeriesDialog->getTimeSeriesList();
        currentBoundaryCondition->setTimeSeriesChanged(timeSeriesDialog->hasChanges());
    }
}

void BoundaryConditionDialog::on_btnCellColor_clicked() {
    QColor color = QColorDialog::getColor(Qt::white, this, "Select a cell color");
    
    if (color.isValid()) {
        QPixmap px(10, 10);
        px.fill(color);
        
        currentBoundaryCondition->setCellColor(color.name());
        currentBoundaryCondition->getSelectionActor()->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
        hydrodynamicDataDialog->ui->vtkWidget->update();
        ui->btnCellColor->setIcon(px);
    }
}

void BoundaryConditionDialog::btnIndividualObjectPicker_clicked(bool checked) {
    hydrodynamicDataDialog->ui->vtkWidget->togglePicker(checked, PickerMode::EACH_CELL);
    hydrodynamicDataDialog->toggleZoomAreaAction(true);
    hydrodynamicDataDialog->ui->vtkWidget->toggleZoomArea(false);
    
    if (checked) {
        btnMultipleObjectPicker->blockSignals(true);
        btnMultipleObjectPicker->setChecked(false);
        btnMultipleObjectPicker->blockSignals(false);
        hydrodynamicDataDialog->activateWindow();
    } else {
        this->activateWindow();
    }
}

void BoundaryConditionDialog::btnMultipleObjectPicker_clicked(bool checked) {
    hydrodynamicDataDialog->ui->vtkWidget->togglePicker(checked, ui->cbxType->currentText() == "Water Level" ? PickerMode::MULTIPLE_CELL : PickerMode::MULTIPLE_EDGE);
    hydrodynamicDataDialog->toggleZoomAreaAction(checked);
    hydrodynamicDataDialog->ui->vtkWidget->toggleZoomArea(false);
    
    if (checked) {
        btnIndividualObjectPicker->blockSignals(true);
        btnIndividualObjectPicker->setChecked(false);
        btnIndividualObjectPicker->blockSignals(false);
        hydrodynamicDataDialog->activateWindow();
    } else {
        this->activateWindow();
    }
}

void BoundaryConditionDialog::accept() {
    if (!isValid()) {
        return;
    }
    
    BoundaryConditionType boundaryConditionType = BoundaryConditionType::WATER_LEVEL;
    
    if (ui->cbxType->currentText() == "Water Flow") {
        boundaryConditionType = BoundaryConditionType::WATER_FLOW;
    } else if (ui->cbxType->currentText() == "Normal Depth") {
        boundaryConditionType = BoundaryConditionType::NORMAL_DEPTH;
    }
    
    currentBoundaryCondition->setType(boundaryConditionType);
    currentBoundaryCondition->setObjectIds(ui->lblElementIds->text());
    currentBoundaryCondition->setFunction(ui->rdoConstant->isChecked() ? BoundaryConditionFunction::CONSTANT : BoundaryConditionFunction::TIME_SERIES);
    currentBoundaryCondition->setConstantValue(ui->edtConstant->text().toDouble());
    currentBoundaryCondition->setVerticalIntegratedOutflow(ui->chkVIO->isChecked());
    currentBoundaryCondition->setMinimumElevation(ui->edtMinimumElevation->text().toDouble());
    currentBoundaryCondition->setMaximumElevation(ui->edtMaximumElevation->text().toDouble());
    currentBoundaryCondition->setInputModule(InputModule::HYDRODYNAMIC);
    
    if (ui->rdoTimeSeries->isChecked()) {
        currentBoundaryCondition->setTimeSeriesList(timeSeriesList);
    }
    
    configuration->addBoundaryCondition(currentBoundaryCondition);
    
    int row = -1;
    
    if (isNewBoundaryCondition) {
        row = hydrodynamicDataDialog->ui->tblBoundaryConditions->rowCount();
        hydrodynamicDataDialog->ui->tblBoundaryConditions->insertRow(row);
    } else {
        row = hydrodynamicDataDialog->ui->tblBoundaryConditions->currentRow();
        hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor()->highlightBoundaryCondition(currentBoundaryCondition, true);
    }
    
    hydrodynamicDataDialog->ui->tblBoundaryConditions->setItem(row, 0, new QTableWidgetItem(currentBoundaryCondition->getTypeLabel()));
    hydrodynamicDataDialog->ui->tblBoundaryConditions->setItem(row, 1, new QTableWidgetItem(currentBoundaryCondition->getFunctionLabel()));
    
    btnIndividualObjectPicker_clicked(false);
    btnMultipleObjectPicker_clicked(false);
    hydrodynamicDataDialog->toggleZoomAreaAction(true);
    hydrodynamicDataDialog->toggleWidgets(true);
    
    QDialog::accept();
}

void BoundaryConditionDialog::reject() {
    undoChanges();
    QDialog::reject();
}

void BoundaryConditionDialog::closeEvent(QCloseEvent *event) {
    undoChanges();
    QDialog::closeEvent(event);
}

void BoundaryConditionDialog::btnClearSelection_clicked() {
    QString question = tr("Are you sure you want to clear the current selection?");
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("Boundary Condition"), question);
    
    if (button == QMessageBox::Yes) {
        hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor()->clearSelection();
        ui->lblElementIds->setText("-");
    }
}

void BoundaryConditionDialog::showObjectIds() {
    QSet<vtkIdType> selectedCellsIds = currentBoundaryCondition->getObjectIds();
    
    if (selectedCellsIds.isEmpty()) {
        ui->lblElementIds->setText("-");
    } else {
        QStringList cells;
        
        for (vtkIdType v : selectedCellsIds) {
            cells.append(QString::number(v));
        }
        
        ui->lblElementIds->setText(cells.join(","));
    }
}

bool BoundaryConditionDialog::isValid() {
    if (currentBoundaryCondition->getObjectIds().isEmpty()) {
        QString message = tr("Please pick at least one %1 from the grid.").arg(ui->cbxType->currentText() == "Water Level" ? "cell" : "edge");
        QMessageBox::warning(this, tr("Boundary Condition"), message);
        return false;
    }
    
    if (ui->rdoConstant->isChecked() && ui->edtConstant->text().isEmpty()) {
        QMessageBox::warning(this, tr("Boundary Condition"), tr("Please input a valid constant value."));
        return false;
    }
    
    if (ui->rdoTimeSeries->isChecked() && timeSeriesList.isEmpty()) {
        QMessageBox::warning(this, tr("Boundary Condition"), tr("Please provide a valid time series list."));
        return false;
    }
    
    if (ui->edtMinimumElevation->isEnabled()) {
        if (ui->edtMinimumElevation->text().isEmpty() || ui->edtMaximumElevation->text().isEmpty()) {
            QMessageBox::warning(this, tr("Boundary Condition"), tr("Elevation interval can't be blank."));
            return false;
        } else if (ui->edtMinimumElevation->text().toDouble() > ui->edtMaximumElevation->text().toDouble()) {
            QMessageBox::warning(this, tr("Boundary Condition"), tr("Invalid elevation interval."));
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
            QMessageBox::warning(this, tr("Boundary Condition"), tr("Minimum elevation must be greater than or equal to %1.").arg(minimumWeight));
            return false;
        }
    }
    
    return true;
}

void BoundaryConditionDialog::toggleLabelsActor(bool show) {
    currentBoundaryCondition->getLabelsActor()->SetVisibility(show);
}

void BoundaryConditionDialog::undoChanges() {
    if (isNewBoundaryCondition) {
        hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor()->removeBoundaryCondition(currentBoundaryCondition);
    } else {
        currentBoundaryCondition->setObjectIds(originalObjectIds);
        currentBoundaryCondition->setTimeSeriesList(originalTimeSeriesList);
        hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor()->renderBoundaryCondition(currentBoundaryCondition);
        hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor()->highlightBoundaryCondition(currentBoundaryCondition, true);
    }
    
    hydrodynamicDataDialog->ui->vtkWidget->togglePicker(false);
    hydrodynamicDataDialog->toggleWidgets(true);
}