#include "include/ui/boundary_condition_dialog.h"
#include "ui_boundary_condition_dialog.h"

#include <QIcon>

#include "include/ui/time_series_dialog.h"

enum class Picker { SINGLE = 1, MULTIPLE };

BoundaryConditionDialog::BoundaryConditionDialog(QWidget *parent) : QDialog(parent), ui(new Ui::BoundaryConditionDialog) {
	ui->setupUi(this);

	btnSingleCellPicker = new QToolButton(this);
    btnSingleCellPicker->setCheckable(true);
	btnSingleCellPicker->setIcon(QIcon(":/icons/cell-picker-single.png"));
	btnSingleCellPicker->setToolTip("Single cell picker");
    btnSingleCellPicker->setProperty("ActionRole", (int) Picker::SINGLE);
	ui->buttonBox->addButton(btnSingleCellPicker, QDialogButtonBox::ActionRole);
    connect(btnSingleCellPicker, SIGNAL(clicked(bool)), this, SLOT(btnSingleCellPicker_clicked(bool)));

	btnMultipleCellPicker = new QToolButton(this);
    btnMultipleCellPicker->setCheckable(true);
	btnMultipleCellPicker->setIcon(QIcon(":/icons/cell-picker-multiple.png"));
	btnMultipleCellPicker->setToolTip("Multiple cell picker");
    btnMultipleCellPicker->setProperty("ActionRole", (int) Picker::MULTIPLE);
	ui->buttonBox->addButton(btnMultipleCellPicker, QDialogButtonBox::ActionRole);
    connect(btnMultipleCellPicker, SIGNAL(clicked(bool)), this, SLOT(btnMultipleCellPicker_clicked(bool)));
}

BoundaryConditionDialog::~BoundaryConditionDialog() {
    delete ui;
}

void BoundaryConditionDialog::setHydrodynamicDataDialog(HydrodynamicDataDialog *dialog) {
    this->hydrodynamicDataDialog = dialog;
}

void BoundaryConditionDialog::on_rdoWaterLevel_clicked(bool checked) {
    ui->lblElementLabel->setText("Cells");
}

void BoundaryConditionDialog::on_rdoWaterFlow_clicked(bool checked) {
    ui->lblElementLabel->setText("Edges");
}

void BoundaryConditionDialog::on_btnTimeSeries_clicked() {
    TimeSeriesDialog *timeSeriesDialog = new TimeSeriesDialog(this);
    timeSeriesDialog->exec();
}

void BoundaryConditionDialog::btnSingleCellPicker_clicked(bool checked) {
    btnMultipleCellPicker->setChecked(false);
    
    hydrodynamicDataDialog->togglePicker(checked, CellPickMode::INDIVIDUAL);
    
    if (checked) {
        hydrodynamicDataDialog->activateWindow();
    } else {
        this->activateWindow();
    }
}

void BoundaryConditionDialog::btnMultipleCellPicker_clicked(bool checked) {
    btnSingleCellPicker->setChecked(false);
    
    hydrodynamicDataDialog->togglePicker(checked, CellPickMode::MULTIPLE);
    
    if (checked) {
        hydrodynamicDataDialog->activateWindow();
    } else {
        this->activateWindow();
    }
}

void BoundaryConditionDialog::accept() {
    hydrodynamicDataDialog->toggleWidgets(true);
    QDialog::accept();
}

void BoundaryConditionDialog::reject() {
    hydrodynamicDataDialog->toggleWidgets(true);
    QDialog::reject();
}

void BoundaryConditionDialog::closeEvent(QCloseEvent *event) {
    hydrodynamicDataDialog->toggleWidgets(true);
    QDialog::closeEvent(event);
}
