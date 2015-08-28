#include "include/ui/create_simulation_dialog.h"
#include "ui_create_simulation_dialog.h"

#include "include/application/iph_application.h"
#include "include/services/simulation_service.h"

#include <QDateTime>
#include <QMessageBox>

CreateSimulationDialog::CreateSimulationDialog(QWidget *parent) :
	QDialog(parent), ui(new Ui::CreateSimulationDialog)
{
	ui->setupUi(this);
	ui->edtInitialTime->setDateTime(QDateTime::currentDateTime());
    
    Project *project = IPHApplication::getCurrentProject();
    
    for (HydrodynamicConfiguration *configuration : project->getHydrodynamicConfigurations()) {
        ui->cbxHydrodynamic->addItem(configuration->getName());
    }
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

//	if (ui->cbxWaterQuality->currentIndex() == -1) {
//		QMessageBox::warning(this, tr("Create Simulation"), tr("Water quality data can't be blank."));
//		return false;
//	}
//
//	if (ui->cbxSediment->currentIndex() == -1) {
//		QMessageBox::warning(this, tr("Create Simulation"), tr("Sediment data can't be blank."));
//		return false;
//	}
//
//	if (ui->cbxMeteorological->currentIndex() == -1) {
//		QMessageBox::warning(this, tr("Create Simulation"), tr("Meteorological data can't be blank."));
//		return false;
//	}

	return true;
}

void CreateSimulationDialog::accept() {
    if (!isValid()) {
        return;
    }
    
    QList<double> layers;
    Simulation *simulation = new Simulation();
    bool startOnCreate = ui->chkStart->isChecked();
    Project *project = IPHApplication::getCurrentProject();
    HydrodynamicConfiguration *hydrodynamicConfiguration = project->getHydrodynamicConfiguration(ui->cbxHydrodynamic->currentText());
    
    simulation->setLabel(ui->edtLabel->text());
    simulation->setSimulationType(Simulation::getSimulationTypesMap().key(ui->cbxType->currentText()));
    simulation->setInitialTime(ui->edtInitialTime->dateTime().toTime_t());
    simulation->setPeriod(ui->edtPeriod->text().toDouble());
    simulation->setStepTime(ui->edtStepTime->text().toInt());
    
    for (int i = 0; i < ui->tblLayers->rowCount(); i++) {
        layers.append(ui->tblLayers->item(i, 0)->text().toDouble());
    }
    
    simulation->setLayers(layers);
    simulation->setHydrodynamicConfiguration(hydrodynamicConfiguration);
    simulation->setObservation(ui->txtObservations->toPlainText());
    simulation->setStartOnCreate(startOnCreate);
    
    project->addSimulation(simulation);
    
    // TODO: run inside a thread
    if (startOnCreate) {
        SimulationService *simulationService = SimulationService::getInstance();
        simulationService->run(simulation);
    }
    
    QDialog::accept();
}

void CreateSimulationDialog::on_btnAddLayer_clicked() {
    ui->tblLayers->insertRow(ui->tblLayers->rowCount());
}

void CreateSimulationDialog::on_btnRemoveLayer_clicked() {
    int row = ui->tblLayers->currentRow();
    
    if (row != -1 && QMessageBox::question(this, tr("Create Simulation"), tr("Are you sure?")) == QMessageBox::Yes) {
        ui->tblLayers->removeRow(row);
    }
}