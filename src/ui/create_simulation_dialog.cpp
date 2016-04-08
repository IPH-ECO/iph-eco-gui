#include <ui/create_simulation_dialog.h>
#include "ui_create_simulation_dialog.h"

#include <application/iph_application.h>
#include <exceptions/database_exception.h>
#include <ui/main_window.h>
#include <application/simulation_manager.h>
#include <repository/project_repository.h>
#include <repository/simulation_repository.h>

#include <QDir>
#include <QRegExp>
#include <QFileInfo>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegExpValidator>

CreateSimulationDialog::CreateSimulationDialog(QWidget *parent) :
	QDialog(parent), ui(new Ui::CreateSimulationDialog)
{
	ui->setupUi(this);
    ui->edtLabel->setValidator(new QRegExpValidator(QRegExp("\\w+"), this));
    ui->edtInitialTime->setDateTime(QDateTime(QDate(QDate::currentDate().year(), 1, 1)));
    ui->trOutputVariables->header()->close();
    
    Project *project = IPHApplication::getCurrentProject();
    
    for (HydrodynamicConfiguration *configuration : project->getHydrodynamicConfigurations()) {
        ui->cbxHydrodynamic->addItem(configuration->getName());
    }
    
    for (MeteorologicalConfiguration *configuration : project->getMeteorologicalConfigurations()) {
        ui->cbxMeteorological->addItem(configuration->getName());
    }
    
    if (project->isPersisted()) {
        QFileInfo fileInfo(project->getFilename());
        ui->edtOutputDirectory->setText(fileInfo.absolutePath());
    }
    
    SimulationRepository::loadOutputParametersTree(ui->trOutputVariables);
    
    QTreeWidgetItemIterator it(ui->trOutputVariables, QTreeWidgetItemIterator::All);
    
    while (*it) {
        (*it)->setExpanded(true);
        it++;
    }
    
    it = QTreeWidgetItemIterator(ui->trOutputVariables, QTreeWidgetItemIterator::All);
    
    while (*it) {
        QTreeWidgetItem *item = *it;
        
        if (item->childCount() == 0) {
            QFont font = item->font(0);
            
            font.setBold(true);
            item->setFont(0, font);
        }
        item->setExpanded(true);
        
        it++;
    }
    
    ui->cbxTemplate->blockSignals(true);
    for (Simulation *simulation : project->getSimulations()) {
        ui->cbxTemplate->addItem(simulation->getLabel());
    }
    ui->cbxTemplate->setCurrentIndex(-1);
    ui->cbxTemplate->blockSignals(false);
}

CreateSimulationDialog::~CreateSimulationDialog() {
    delete ui;
}

bool CreateSimulationDialog::isValid() {
    Project *project = IPHApplication::getCurrentProject();
    
    if (ui->edtLabel->text().isEmpty()) {
		QMessageBox::warning(this, tr("Create Simulation"), tr("Label can't be blank."));
		return false;
	}
    
    if (project->getSimulation(ui->edtLabel->text())) {
        QMessageBox::warning(this, tr("Create Simulation"), tr("Label already used."));
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

	if (ui->cbxMeteorological->currentIndex() == -1) {
		QMessageBox::warning(this, tr("Create Simulation"), tr("Meteorological data can't be blank."));
		return false;
	}
    
    if (!QDir(ui->edtOutputDirectory->text()).exists()) {
        QMessageBox::warning(this, tr("Create Simulation"), tr("The specified output directory does not exist."));
        return false;
    }
    
    HydrodynamicConfiguration *hydrodynamicConfiguration = project->getHydrodynamicConfiguration(ui->cbxHydrodynamic->currentText());
    QDateTime initialTime = ui->edtInitialTime->dateTime();
    QDateTime endTime = initialTime.addDays(ui->edtPeriod->text().toInt());
    
    initialTime.setTimeSpec(Qt::UTC);
    endTime.setTimeSpec(Qt::UTC);
    
    uint initialTimeStamp = initialTime.toTime_t();
    uint endTimeStamp = endTime.toTime_t();
    uint minimumTimeStamp = 0;
    uint maximumTimeStamp = 0;
    
    for (BoundaryCondition *boundaryCondition : hydrodynamicConfiguration->getBoundaryConditions()) {
        if (boundaryCondition->getFunction() == BoundaryConditionFunction::TIME_SERIES) {
            TimeSeries *firstEntry = boundaryCondition->getTimeSeriesList().first();
            
            if (minimumTimeStamp == 0) {
                minimumTimeStamp = firstEntry->getTimeStamp();
                maximumTimeStamp = firstEntry->getTimeStamp();
            }
            
            for (TimeSeries *timeSeries : boundaryCondition->getTimeSeriesList()) {
                uint timeStamp = (uint) timeSeries->getTimeStamp();
                
                if (minimumTimeStamp > timeStamp) {
                    minimumTimeStamp = timeStamp;
                }
                
                if (maximumTimeStamp < timeStamp) {
                    maximumTimeStamp = timeStamp;
                }
            }
        }
    }
    
    if (minimumTimeStamp != 0 && initialTimeStamp < minimumTimeStamp) {
        initialTime = QDateTime::fromTime_t(minimumTimeStamp, Qt::UTC);
        QMessageBox::warning(this, tr("Create Simulation"), QString("Initial time must be the same or greater than %1.").arg(initialTime.toString("yyyy-MM-dd HH:mm:ss")));
        return false;
    }
    
    if (maximumTimeStamp != 0 && endTimeStamp > maximumTimeStamp) {
        endTime = QDateTime::fromTime_t(maximumTimeStamp, Qt::UTC);
        QMessageBox::warning(this, tr("Create Simulation"), QString("Total simulation time must be the same or lesser than %1.").arg(endTime.toString("yyyy-MM-dd HH:mm:ss")));
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
    
    if (!project->isPersisted()) {
        QMessageBox::information(this, tr("Create Simulation"), tr("This project must be saved before to create a simulation."));
        MainWindow *mainWindow = static_cast<MainWindow*>(this->parent());
        mainWindow->on_actionSaveProject_triggered();
        return false;
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
    simulation->setOutputTimeInterval(ui->edtOutputTimeInterval->text().toInt());
    simulation->setAutosaveTimeInterval(ui->edtAutosaveInterval->text().toInt());

    QTreeWidgetItemIterator it(ui->trOutputVariables, QTreeWidgetItemIterator::Checked);
    QList<QString> parameters;

    while (*it) {
        QString parameter = (*it)->data(0, Qt::UserRole).toString();
        parameters.append(parameter);
        it++;
    }

    simulation->setOutputParameters(parameters);
    
    if (startOnCreate) {
        simulation->setStartTime(QDateTime::currentDateTimeUtc().toTime_t());
    }
    
    try {
        ProjectRepository projectRepository(project->getFilename());
        SimulationManager *simulationManager = SimulationManager::getInstance();
        
        projectRepository.saveSimulation(simulation);
        
        if (startOnCreate) {
            simulationManager->start(simulation);
        } else {
            simulationManager->addIdle(simulation);
        }
        
        project->addSimulation(simulation); // Must be added after start to handle the simulation
    } catch (DatabaseException &ex) {
        QMessageBox::critical(this, "Create Simulation", ex.what());
        return;
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

void CreateSimulationDialog::on_cbxTemplate_currentTextChanged(const QString &simulationLabel) {
    Project *project = IPHApplication::getCurrentProject();
    Simulation *simulation = project->getSimulation(simulationLabel);
    
    ui->cbxType->setCurrentText(Simulation::getSimulationTypesMap().value(simulation->getSimulationType()));
    ui->edtInitialTime->setDateTime(simulation->getInitialTimeAsDateTime());
    ui->edtPeriod->setText(QString::number(simulation->getPeriod()));
    ui->edtStepTime->setText(QString::number(simulation->getStepTime()));
    ui->cbxHydrodynamic->setCurrentText(simulation->getHydrodynamicConfiguration()->getName());
//    ui->cbxWaterQuality->setCurrentText(simulation->getWaterQualityConfiguration()->getName());
    ui->cbxMeteorological->setCurrentText(simulation->getMeteorologicalConfiguration()->getName());
    ui->edtMinLimit->setText(QString::number(simulation->getMinimumVerticalLimit()));
    ui->edtMaxLimit->setText(QString::number(simulation->getMaximumVerticalLimit()));
    
    for (double layer : simulation->getLayers()) {
        int row = ui->tblLayers->rowCount();
        ui->tblLayers->insertRow(row);
        ui->tblLayers->setItem(row, 0, new QTableWidgetItem(QString::number(layer)));
    }
    
    ui->edtOutputDirectory->setText(simulation->getOutputDirectory());
    ui->edtOutputTimeInterval->setText(QString::number(simulation->getOutputTimeInterval()));
    ui->edtAutosaveInterval->setText(QString::number(simulation->getAutosaveTimeInterval()));
    
    QTreeWidgetItemIterator it(ui->trOutputVariables, QTreeWidgetItemIterator::NoChildren);
    
    while (*it) {
        QString parameterName = (*it)->data(0, Qt::UserRole).toString();
        (*it)->setCheckState(0, simulation->getOutputParameters().contains(parameterName) ? Qt::Checked : Qt::Unchecked);
        it++;
    }
}