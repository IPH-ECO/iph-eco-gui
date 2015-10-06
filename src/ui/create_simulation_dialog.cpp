#include "include/ui/create_simulation_dialog.h"
#include "ui_create_simulation_dialog.h"

#include "include/application/iph_application.h"
#include "include/services/simulation_service.h"

#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>

CreateSimulationDialog::CreateSimulationDialog(QWidget *parent) :
	QDialog(parent), ui(new Ui::CreateSimulationDialog)
{
	ui->setupUi(this);
    ui->edtInitialTime->setDateTime(QDateTime(QDate(QDate::currentDate().year(), 1, 1)));
    
    Project *project = IPHApplication::getCurrentProject();
    
    for (HydrodynamicConfiguration *configuration : project->getHydrodynamicConfigurations()) {
        ui->cbxHydrodynamic->addItem(configuration->getName());
    }
    
    for (MeteorologicalConfiguration *configuration : project->getMeteorologicalConfigurations()) {
        ui->cbxMeteorological->addItem(configuration->getName());
    }
    
    QFileInfo fileInfo(project->getFilename());
    ui->edtOutputDirectory->setText(fileInfo.absolutePath());
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

	if (ui->cbxMeteorological->currentIndex() == -1) {
		QMessageBox::warning(this, tr("Create Simulation"), tr("Meteorological data can't be blank."));
		return false;
	}
    
    if (!QDir(ui->edtOutputDirectory->text()).exists()) {
        QMessageBox::warning(this, tr("Create Simulation"), tr("The specified output directory does not exist."));
        return false;
    }
    
    Project *project = IPHApplication::getCurrentProject();
    HydrodynamicConfiguration *hydrodynamicConfiguration = project->getHydrodynamicConfiguration(ui->cbxHydrodynamic->currentText());
    QDateTime time = ui->edtInitialTime->dateTime();
    uint initialTimeStamp = time.toTime_t();
    uint minimumTimeStamp = 0;
    
    time.setTimeSpec(Qt::UTC);
    
    for (BoundaryCondition *boundaryCondition : hydrodynamicConfiguration->getBoundaryConditions()) {
        if (boundaryCondition->getFunction() == BoundaryConditionFunction::TIME_SERIES) {
            TimeSeries *firstEntry = boundaryCondition->getTimeSeriesList().first();
            
            if (minimumTimeStamp == 0) {
                minimumTimeStamp = firstEntry->getTimeStamp();
            }
            
            if (minimumTimeStamp > (uint) firstEntry->getTimeStamp()) {
                minimumTimeStamp = firstEntry->getTimeStamp();
            }
        }
    }
    
    if (minimumTimeStamp != 0 && initialTimeStamp < minimumTimeStamp) {
        time = QDateTime::fromTime_t(minimumTimeStamp, Qt::UTC);
        QMessageBox::warning(this, tr("Create Simulation"), QString("Initial time must be the same or greater than %1.").arg(time.toString("yyyy-MM-dd HH:mm:ss")));
        return false;
    }

    if (ui->tblLayers->rowCount() > 0) {
        double minLimit = ui->edtMinLimit->text().toDouble();
        double maxLimit = ui->edtMaxLimit->text().toDouble();
        
        for (int i = 0; i < ui->tblLayers->rowCount(); i++) {
            double depth = ui->tblLayers->item(i, 0)->text().toDouble();
            
            if (depth < minLimit || depth > maxLimit) {
                QMessageBox::warning(this, tr("Create Simulation"), QString("Depth %1 is out of range (line %2).").arg(depth).arg(i + 1));
                return false;
            }
        }
    }

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
    MeteorologicalConfiguration *meteorologicalConfiguration = project->getMeteorologicalConfiguration(ui->cbxMeteorological->currentText());
    QDateTime time = ui->edtInitialTime->dateTime();
    
    time.setTimeSpec(Qt::UTC);
    
    // Settings tab
    simulation->setLabel(ui->edtLabel->text());
    simulation->setSimulationType(Simulation::getSimulationTypesMap().key(ui->cbxType->currentText()));
    simulation->setInitialTime(time.toTime_t());
    simulation->setPeriod(ui->edtPeriod->text().toDouble());
    simulation->setStepTime(ui->edtStepTime->text().toInt());
    simulation->setMinimumVerticalLimit(ui->edtMinLimit->text().toDouble());
    simulation->setMaximumVerticalLimit(ui->edtMaxLimit->text().toDouble());
    
    for (int i = 0; i < ui->tblLayers->rowCount(); i++) {
        layers.append(ui->tblLayers->item(i, 0)->text().toDouble());
    }
    
    simulation->setLayers(layers);
    simulation->setHydrodynamicConfiguration(hydrodynamicConfiguration);
    simulation->setMeteorologicalConfiguration(meteorologicalConfiguration);
    simulation->setObservation(ui->txtObservations->toPlainText());
    simulation->setStartOnCreate(startOnCreate);
    
    // Output tab
    simulation->setOutputDirectory(ui->edtOutputDirectory->text());
    
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

void CreateSimulationDialog::on_cbxHydrodynamic_currentTextChanged(const QString &configurationName) {
    Project *project = IPHApplication::getCurrentProject();
    HydrodynamicConfiguration *hydrodynamicConfiguration = project->getHydrodynamicConfiguration(configurationName);
    GridDataConfiguration *gridDataConfiguration = hydrodynamicConfiguration->getGridDataConfiguration();
    GridData *gridData = gridDataConfiguration->getGridData(GridDataType::BATHYMETRY).first();
    
    ui->edtMinLimit->setText(QString::number(gridData->getMinimumWeight()));
}

void CreateSimulationDialog::on_btnBrowseOutputDirectory_clicked() {
    QFileDialog fileDialog(this, tr("Select a output directory"));
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOption(QFileDialog::ShowDirsOnly);
    int exitCode = fileDialog.exec();
    
    if (exitCode == QDialog::Accepted) {
        ui->edtOutputDirectory->setText(fileDialog.selectedFiles().first());
    }
}