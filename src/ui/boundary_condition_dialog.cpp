#include "include/ui/boundary_condition_dialog.h"
#include "ui_boundary_condition_dialog.h"
#include "ui_hydrodynamic_data_dialog.h"

#include <QIcon>
#include <QStringList>
#include <QMessageBox>
#include <QColorDialog>
#include <vtkProperty.h>

#include "include/ui/hydrodynamic_vtk_widget.h"
#include "include/ui/time_series_dialog.h"

enum class CustomButtonRole { INDIVIDUAL_CELL_PICKER = 1, MULTIPLE_CELL_PICKER, CLEAR_SELECTION };

BoundaryConditionDialog::BoundaryConditionDialog(HydrodynamicConfiguration *configuration, BoundaryCondition *boundaryCondition) :
    QDialog(nullptr), ui(new Ui::BoundaryConditionDialog), configuration(configuration), currentBoundaryCondition(boundaryCondition), isNewBoundaryCondition(boundaryCondition == nullptr)
{
	ui->setupUi(this);
    this->setWindowModality(Qt::WindowModal);

	btnIndividualCellPicker = new QToolButton(this);
    btnIndividualCellPicker->setCheckable(true);
	btnIndividualCellPicker->setIcon(QIcon(":/icons/cell-picker-individual"));
	btnIndividualCellPicker->setToolTip("Individual cell picker");
    btnIndividualCellPicker->setProperty("ActionRole", (int) CustomButtonRole::INDIVIDUAL_CELL_PICKER);
	ui->buttonBox->addButton(btnIndividualCellPicker, QDialogButtonBox::ActionRole);
    connect(btnIndividualCellPicker, SIGNAL(clicked(bool)), this, SLOT(btnIndividualCellPicker_clicked(bool)));

	btnMultipleCellPicker = new QToolButton(this);
    btnMultipleCellPicker->setCheckable(true);
	btnMultipleCellPicker->setIcon(QIcon(":/icons/cell-picker-multiple.png"));
	btnMultipleCellPicker->setToolTip("Multiple cell picker");
    btnMultipleCellPicker->setProperty("ActionRole", (int) CustomButtonRole::MULTIPLE_CELL_PICKER);
	ui->buttonBox->addButton(btnMultipleCellPicker, QDialogButtonBox::ActionRole);
    ui->buttonBox->setCenterButtons(true);
    connect(btnMultipleCellPicker, SIGNAL(clicked(bool)), this, SLOT(btnMultipleCellPicker_clicked(bool)));
    
    QToolButton *btnClearSelection = new QToolButton(this);
    btnClearSelection->setIcon(QIcon(":/icons/edit-clear-2.png"));
    btnClearSelection->setToolTip("Clear selection");
    btnClearSelection->setProperty("ActionRole", (int) CustomButtonRole::CLEAR_SELECTION);
    ui->buttonBox->addButton(btnClearSelection, QDialogButtonBox::ActionRole);
    connect(btnClearSelection, SIGNAL(clicked()), this, SLOT(btnClearSelection_clicked()));
    
    if (currentBoundaryCondition) {
        bool isWaterLevelSelected = boundaryCondition->getType() == BoundaryConditionType::WATER_LEVEL;
        bool isConstantSelected = boundaryCondition->getFunction() == BoundaryConditionFunction::CONSTANT;
        bool useVerticalIntegratedOutflow = boundaryCondition->useVerticalIntegratedOutflow();
        
        ui->rdoWaterLevel->setChecked(isWaterLevelSelected);
        ui->rdoWaterFlow->setChecked(!isWaterLevelSelected);
        ui->lblElementLabel->setText(isWaterLevelSelected ? "Cells" : "Edges");
        ui->lblElementIds->setText(boundaryCondition->getObjectIdsStr());
        ui->rdoConstant->setChecked(isConstantSelected);
        ui->rdoTimeSeries->setChecked(!isConstantSelected);
        ui->edtConstant->setEnabled(isConstantSelected);
        if (isConstantSelected) {
            ui->edtConstant->setText(QString::number(boundaryCondition->getConstantValue()));
        } else {
            ui->btnTimeSeries->setEnabled(boundaryCondition->getFunction() == BoundaryConditionFunction::TIME_SERIES);
        }
        ui->chkVIO->setEnabled(!isWaterLevelSelected);
        ui->chkVIO->setChecked(useVerticalIntegratedOutflow);
        ui->edtQuota->setEnabled(!useVerticalIntegratedOutflow);
        if (!useVerticalIntegratedOutflow) {
            ui->edtQuota->setText(QString::number(boundaryCondition->getQuota()));
        }
    } else {
        this->currentBoundaryCondition = new BoundaryCondition();
    }
    
    QColor color = QColor(this->currentBoundaryCondition->getCellColor());
    QPixmap px(10, 10);
    
    px.fill(color);
    ui->btnCellColor->setIcon(px);
    
    this->originalObjectIds = this->currentBoundaryCondition->getObjectIds();
}

BoundaryConditionDialog::~BoundaryConditionDialog() {
    delete ui;
}

void BoundaryConditionDialog::setHydrodynamicDataDialog(HydrodynamicDataDialog *dialog) {
    this->hydrodynamicDataDialog = dialog;
    this->hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor()->setBoundaryCondition(currentBoundaryCondition);
    this->currentBoundaryCondition->getLabelsActor()->SetVisibility(hydrodynamicDataDialog->ui->btnShowCellLabels->isChecked());
    
    connect(hydrodynamicDataDialog->ui->btnShowCellLabels, SIGNAL(clicked(bool)), this, SLOT(toggleLabelsActor(bool)));
    connect(hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor(), SIGNAL(objectSelected()), this, SLOT(showObjectIds()));
}

void BoundaryConditionDialog::on_rdoWaterLevel_clicked(bool checked) {
    ui->lblElementLabel->setText("Cells");
    ui->lblElementIds->setText("-");
    
    if (!currentBoundaryCondition->getObjectIds().isEmpty()) {
        QString question = tr("Changing the condition type will remove the selected objects. Are you sure?");
        QMessageBox::StandardButton button = QMessageBox::question(this, tr("Boundary Condition"), question);
        
        if (button == QMessageBox::No) {
            ui->lblElementLabel->setText("Edges");
            ui->lblElementIds->setText(currentBoundaryCondition->getObjectIdsStr());
            ui->rdoWaterFlow->setChecked(true);
        } else {
            currentBoundaryCondition->clearObjectIds();
            hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor()->renderBoundaryCondition(currentBoundaryCondition);
        }
    }
}

void BoundaryConditionDialog::on_rdoWaterFlow_clicked(bool checked) {
    ui->lblElementLabel->setText("Edges");
    ui->lblElementIds->setText("-");
    
    if (!currentBoundaryCondition->getObjectIds().isEmpty()) {
        QString question = tr("Changing the condition type will remove the selected objects. Are you sure?");
        QMessageBox::StandardButton button = QMessageBox::question(this, tr("Boundary Condition"), question);
        
        if (button == QMessageBox::No) {
            ui->lblElementLabel->setText("Cells");
            ui->lblElementIds->setText(currentBoundaryCondition->getObjectIdsStr());
            ui->rdoWaterLevel->setChecked(true);
        } else {
            currentBoundaryCondition->clearObjectIds();
            hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor()->renderBoundaryCondition(currentBoundaryCondition);
        }
    }
}

void BoundaryConditionDialog::on_btnTimeSeries_clicked() {
    TimeSeriesDialog *timeSeriesDialog = new TimeSeriesDialog(this, currentBoundaryCondition);
    timeSeriesDialog->exec();
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

void BoundaryConditionDialog::btnIndividualCellPicker_clicked(bool checked) {
    btnMultipleCellPicker->setChecked(false);
    
    hydrodynamicDataDialog->ui->vtkWidget->togglePicker(checked, CellPickMode::INDIVIDUAL);
    
    if (checked) {
        hydrodynamicDataDialog->activateWindow();
    } else {
        this->activateWindow();
    }
}

void BoundaryConditionDialog::btnMultipleCellPicker_clicked(bool checked) {
    btnIndividualCellPicker->setChecked(false);
    
    hydrodynamicDataDialog->ui->vtkWidget->togglePicker(checked, CellPickMode::MULTIPLE);
    
    if (checked) {
        hydrodynamicDataDialog->activateWindow();
    } else {
        this->activateWindow();
    }
}

void BoundaryConditionDialog::accept() {
    if (!isValid()) {
        return;
    }
    
    currentBoundaryCondition->setType(ui->rdoWaterLevel->isChecked() ? BoundaryConditionType::WATER_LEVEL : BoundaryConditionType::WATER_FLOW);
    currentBoundaryCondition->setObjectIds(ui->lblElementIds->text());
    currentBoundaryCondition->setFunction(ui->rdoConstant->isChecked() ? BoundaryConditionFunction::CONSTANT : BoundaryConditionFunction::TIME_SERIES);
    currentBoundaryCondition->setConstantValue(ui->edtConstant->text().toDouble());
    currentBoundaryCondition->setVerticalIntegratedOutflow(ui->chkVIO->isChecked());
    currentBoundaryCondition->setQuota(ui->edtQuota->text().toDouble());
    currentBoundaryCondition->setInputModule(InputModule::HYDRODYNAMIC);
    
    configuration->addBoundaryCondition(currentBoundaryCondition);
    
    int row = -1;
    
    if (isNewBoundaryCondition) {
        row = hydrodynamicDataDialog->ui->tblBoundaryConditions->rowCount();
        hydrodynamicDataDialog->ui->tblBoundaryConditions->insertRow(row);
    } else {
        row = hydrodynamicDataDialog->ui->tblBoundaryConditions->currentRow();
    }
    
    hydrodynamicDataDialog->ui->tblBoundaryConditions->setItem(row, 0, new QTableWidgetItem(currentBoundaryCondition->getTypeStr()));
    hydrodynamicDataDialog->ui->tblBoundaryConditions->setItem(row, 1, new QTableWidgetItem(currentBoundaryCondition->getFunctionStr()));
    
    btnIndividualCellPicker_clicked(false);
    btnMultipleCellPicker_clicked(false);
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
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("Hydrodynamic Data"), question);
    
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
        QMessageBox::warning(this, tr("Boundary Condition"), tr("Please pick at least one cell or edge from the grid."));
        return false;
    }
    
    if (ui->rdoConstant->isChecked() && ui->edtConstant->text().isEmpty()) {
        QMessageBox::warning(this, tr("Boundary Condition"), tr("Please input a valid constant value."));
        return false;
    }
    
    if (ui->edtQuota->isEnabled() && ui->edtQuota->text().isEmpty()) {
        QMessageBox::warning(this, tr("Boundary Condition"), tr("Please input a valid quota value."));
        return false;
    }
    
    return true;
}

void BoundaryConditionDialog::toggleLabelsActor(bool show) {
    currentBoundaryCondition->getLabelsActor()->SetVisibility(show);
}

void BoundaryConditionDialog::undoChanges() {
    if (isNewBoundaryCondition) {
        hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor()->removeBoundaryCondition(currentBoundaryCondition);
        delete currentBoundaryCondition;
    } else {
        currentBoundaryCondition->setObjectIds(originalObjectIds);
        hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor()->renderBoundaryCondition(currentBoundaryCondition);
    }
    
    hydrodynamicDataDialog->ui->vtkWidget->togglePicker(false);
    hydrodynamicDataDialog->toggleWidgets(true);
}