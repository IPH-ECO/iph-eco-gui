#include "include/ui/create_simulation_dialog.h"
#include "ui_create_simulation_dialog.h"

#include <QDateTime>
#include <QMessageBox>

CreateSimulationDialog::CreateSimulationDialog(QWidget *parent) :
	QDialog(parent), ui(new Ui::CreateSimulationDialog), unsavedSimulation(new Simulation), currentSimulation(unsavedSimulation)
{
	ui->setupUi(this);
	ui->edtInitialTime->setDateTime(QDateTime::currentDateTime());
}

CreateSimulationDialog::~CreateSimulationDialog() {
    delete ui;
}

bool CreateSimulationDialog::isValid() {
	if (ui->edtLabel->text().isEmpty()) {
		QMessageBox::warning(this, tr("Create Simulation"), tr("Label can't be blank."));
		return false;
	}

	if (ui->edtInitialTime->text().isEmpty()) {
		QMessageBox::warning(this, tr("Create Simulation"), tr("Initial time can't be blank."));
		return false;
	}

	if (ui->edtPeriod->text().isEmpty()) {
		QMessageBox::warning(this, tr("Create Simulation"), tr("Period can't be blank."));
		return false;
	}

	if (ui->edtStepTime->text().isEmpty()) {
		QMessageBox::warning(this, tr("Create Simulation"), tr("Step time can't be blank."));
		return false;
	}

	if (ui->cbxHydrodynamic->currentIndex() == -1) {
		QMessageBox::warning(this, tr("Create Simulation"), tr("Hydrodynamic data can't be blank."));
		return false;
	}

	if (ui->cbxWaterQuality->currentIndex() == -1) {
		QMessageBox::warning(this, tr("Create Simulation"), tr("Water quality data can't be blank."));
		return false;
	}

	if (ui->cbxSediment->currentIndex() == -1) {
		QMessageBox::warning(this, tr("Create Simulation"), tr("Sediment data can't be blank."));
		return false;
	}

	if (ui->cbxMeteorological->currentIndex() == -1) {
		QMessageBox::warning(this, tr("Create Simulation"), tr("Meteorological data can't be blank."));
		return false;
	}

	return true;
}