#include "include/ui/boundary_condition_dialog.h"
#include "ui_boundary_condition_dialog.h"

#include <QToolButton>
#include <QIcon>

#include "include/ui/time_series_dialog.h"

enum class Picker { SINGLE = 1, MULTIPLE };

BoundaryConditionDialog::BoundaryConditionDialog(QWidget *parent) : QDialog(parent), ui(new Ui::BoundaryConditionDialog) {
	ui->setupUi(this);

	QToolButton *btnSingleCellPicker = new QToolButton();
	btnSingleCellPicker->setIcon(QIcon(":/icons/cell-picker-single.png"));
	btnSingleCellPicker->setToolTip("Single cell picker");
    btnSingleCellPicker->setProperty("ActionRole", (int) Picker::SINGLE);
	ui->buttonBox->addButton(btnSingleCellPicker, QDialogButtonBox::ActionRole);
    connect(btnSingleCellPicker, SIGNAL(clicked()), this, SLOT(btnSingleCellPicker_clicked()));

	QToolButton *btnMultipleCellPicker = new QToolButton();
	btnMultipleCellPicker->setIcon(QIcon(":/icons/cell-picker-multiple.png"));
	btnMultipleCellPicker->setToolTip("Multiple cell picker");
    btnMultipleCellPicker->setProperty("ActionRole", (int) Picker::MULTIPLE);
	ui->buttonBox->addButton(btnMultipleCellPicker, QDialogButtonBox::ActionRole);
    connect(btnMultipleCellPicker, SIGNAL(clicked()), this, SLOT(btnMultipleCellPicker_clicked()));
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

void BoundaryConditionDialog::btnSingleCellPicker_clicked() {
    hydrodynamicDataDialog->activateWindow();
    hydrodynamicDataDialog->setFocus();
}

void BoundaryConditionDialog::btnMultipleCellPicker_clicked() {
    
}