#include "include/ui/boundary_condition_dialog.h"
#include "ui_boundary_condition_dialog.h"
#include "ui_hydrodynamic_data_dialog.h"

#include <QIcon>
#include <QStringList>
#include <QMessageBox>

#include "include/ui/hydrodynamic_vtk_widget.h"
#include "include/ui/time_series_dialog.h"

enum class CustomButtonRole { INDIVIDUAL_CELL_PICKER = 1, MULTIPLE_CELL_PICKER, CLEAR_SELECTION };

BoundaryConditionDialog::BoundaryConditionDialog(HydrodynamicConfiguration *configuration, BoundaryCondition *boundaryCondition) :
    QDialog(nullptr), ui(new Ui::BoundaryConditionDialog), configuration(configuration), boundaryCondition(boundaryCondition), isNewBoundaryCondition(boundaryCondition == nullptr)
{
	ui->setupUi(this);
    this->setWindowModality(Qt::WindowModal);

	btnSingleCellPicker = new QToolButton(this);
    btnSingleCellPicker->setCheckable(true);
	btnSingleCellPicker->setIcon(QIcon(":/icons/cell-picker-single.png"));
	btnSingleCellPicker->setToolTip("Single cell picker");
    btnSingleCellPicker->setProperty("ActionRole", (int) CustomButtonRole::INDIVIDUAL_CELL_PICKER);
	ui->buttonBox->addButton(btnSingleCellPicker, QDialogButtonBox::ActionRole);
    connect(btnSingleCellPicker, SIGNAL(clicked(bool)), this, SLOT(btnSingleCellPicker_clicked(bool)));

	btnMultipleCellPicker = new QToolButton(this);
    btnMultipleCellPicker->setCheckable(true);
	btnMultipleCellPicker->setIcon(QIcon(":/icons/cell-picker-multiple.png"));
	btnMultipleCellPicker->setToolTip("Multiple cell picker");
    btnMultipleCellPicker->setProperty("ActionRole", (int) CustomButtonRole::MULTIPLE_CELL_PICKER);
	ui->buttonBox->addButton(btnMultipleCellPicker, QDialogButtonBox::ActionRole);
    connect(btnMultipleCellPicker, SIGNAL(clicked(bool)), this, SLOT(btnMultipleCellPicker_clicked(bool)));
    
    QToolButton *btnClearSelection = new QToolButton(this);
    btnClearSelection->setIcon(QIcon(":/icons/edit-clear-2.png"));
    btnClearSelection->setToolTip("Clear selection");
    btnClearSelection->setProperty("ActionRole", (int) CustomButtonRole::CLEAR_SELECTION);
    ui->buttonBox->addButton(btnClearSelection, QDialogButtonBox::ActionRole);
    connect(btnClearSelection, SIGNAL(clicked()), this, SLOT(btnClearSelection_clicked()));
    
    if (boundaryCondition) {
        bool isWaterLevelSelected = boundaryCondition->getType() == BoundaryConditionType::WATER_LEVEL;
        bool isConstantSelected = boundaryCondition->getFunction() == BoundaryConditionFunction::CONSTANT;
        
        on_rdoWaterLevel_clicked(isWaterLevelSelected);
        on_rdoWaterFlow_clicked(!isWaterLevelSelected);
        ui->lblElementIds->setText(boundaryCondition->getObjectIdsStr());
        ui->rdoConstant->setChecked(isConstantSelected);
        ui->rdoTimeSeries->setChecked(!isConstantSelected);
        ui->edtConstant->setEnabled(isConstantSelected);
        if (isConstantSelected) {
            ui->edtConstant->setText(QString::number(boundaryCondition->getConstantValue()));
        } else {
            ui->btnTimeSeries->setEnabled(boundaryCondition->getFunction() == BoundaryConditionFunction::TIME_SERIES);
        }
    } else {
        this->boundaryCondition = new BoundaryCondition();
    }
}

BoundaryConditionDialog::~BoundaryConditionDialog() {
    delete ui;
}

void BoundaryConditionDialog::setHydrodynamicDataDialog(HydrodynamicDataDialog *dialog) {
    this->hydrodynamicDataDialog = dialog;
    connect(hydrodynamicDataDialog->ui->vtkWidget->getMouseInteractor(), SIGNAL(objectSelected()), this, SLOT(showObjectIds()));
}

void BoundaryConditionDialog::on_rdoWaterLevel_clicked(bool checked) {
    if (checked) {
        ui->lblElementLabel->setText("Cells");
        ui->lblElementIds->setText("-");
    }
}

void BoundaryConditionDialog::on_rdoWaterFlow_clicked(bool checked) {
    if (checked) {
        ui->lblElementLabel->setText("Edges");
        ui->lblElementIds->setText("-");
    }
}

void BoundaryConditionDialog::on_btnTimeSeries_clicked() {
    TimeSeriesDialog *timeSeriesDialog = new TimeSeriesDialog(this, boundaryCondition);
    timeSeriesDialog->exec();
}

void BoundaryConditionDialog::btnSingleCellPicker_clicked(bool checked) {
    btnMultipleCellPicker->setChecked(false);
    
    hydrodynamicDataDialog->ui->vtkWidget->togglePicker(checked, CellPickMode::INDIVIDUAL);
    
    if (checked) {
        hydrodynamicDataDialog->activateWindow();
    } else {
        this->activateWindow();
    }
}

void BoundaryConditionDialog::btnMultipleCellPicker_clicked(bool checked) {
    btnSingleCellPicker->setChecked(false);
    
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
    
    boundaryCondition->setType(ui->rdoWaterLevel->isChecked() ? BoundaryConditionType::WATER_LEVEL : BoundaryConditionType::WATER_FLOW);
    boundaryCondition->setObjectIds(ui->lblElementIds->text());
    boundaryCondition->setFunction(ui->rdoConstant->isChecked() ? BoundaryConditionFunction::CONSTANT : BoundaryConditionFunction::TIME_SERIES);
    boundaryCondition->setConstantValue(ui->edtConstant->text().toDouble());
    boundaryCondition->setInputModule(InputModule::HYDRODYNAMIC);
    
    configuration->addBoundaryCondition(boundaryCondition);
    
    int row = -1;
    
    if (isNewBoundaryCondition) {
        row = hydrodynamicDataDialog->ui->tblBoundaryConditions->rowCount();
        hydrodynamicDataDialog->ui->tblBoundaryConditions->insertRow(row);
    } else {
        row = hydrodynamicDataDialog->ui->tblBoundaryConditions->currentRow();
    }
    
    hydrodynamicDataDialog->ui->tblBoundaryConditions->setItem(row, 0, new QTableWidgetItem(boundaryCondition->getTypeStr()));
    hydrodynamicDataDialog->ui->tblBoundaryConditions->setItem(row, 1, new QTableWidgetItem(boundaryCondition->getFunctionStr()));
    
    btnSingleCellPicker_clicked(false);
    btnMultipleCellPicker_clicked(false);
    hydrodynamicDataDialog->toggleWidgets(true);
    
    QDialog::accept();
}

void BoundaryConditionDialog::reject() {
    if (isNewBoundaryCondition) {
        delete boundaryCondition;
    }
    
    hydrodynamicDataDialog->ui->vtkWidget->togglePicker(false);
    hydrodynamicDataDialog->toggleWidgets(true);
    QDialog::reject();
}

void BoundaryConditionDialog::closeEvent(QCloseEvent *event) {
    if (isNewBoundaryCondition) {
        delete boundaryCondition;
    }
    
    hydrodynamicDataDialog->ui->vtkWidget->togglePicker(false);
    hydrodynamicDataDialog->toggleWidgets(true);
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
    vtkIdTypeArray *selectedCellIds = hydrodynamicDataDialog->ui->vtkWidget->getSelectedCellIds();
    
    if (selectedCellIds->GetNumberOfTuples() == 0) {
        ui->lblElementIds->setText("-");
    } else {
        QStringList cells;
        
        for (vtkIdType i = 0; i < selectedCellIds->GetNumberOfTuples(); i++) {
            cells.append(QString::number(selectedCellIds->GetTuple1(i)));
        }
        
        ui->lblElementIds->setText(cells.join(","));
    }
}

bool BoundaryConditionDialog::isValid() {
    if (hydrodynamicDataDialog->ui->vtkWidget->getSelectedCellIds()->GetNumberOfTuples() == 0) {
        QMessageBox::warning(this, tr("Boundary Condition"), tr("Please pick at least one cell or edge from the grid."));
        return false;
    }
    
    if (ui->rdoConstant->isChecked() && ui->edtConstant->text().isEmpty()) {
        QMessageBox::warning(this, tr("Boundary Condition"), tr("Please input a valid constant value."));
        return false;
    }
    
    return true;
}