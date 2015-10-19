#include "include/ui/simulation_manager_dialog.h"
#include "ui_simulation_manager_dialog.h"

#include "include/application/iph_application.h"
#include "include/application/simulation_manager.h"
#include "include/ui/main_window.h"

#include <QMessageBox>
#include <QMdiSubWindow>

SimulationManagerDialog::SimulationManagerDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SimulationManagerDialog) {
	ui->setupUi(this);
    ui->tblAll->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblIdle->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblRunning->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblPaused->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblFinished->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblOutput->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	Project *project = IPHApplication::getCurrentProject();
    
    QObject::connect(project, SIGNAL(simulationCreated(Simulation*)), this, SLOT(onSimulationCreated(Simulation*)));

	for (Simulation *simulation : project->getSimulations()) {
		int row = ui->tblAll->rowCount();
        QTableWidgetItem *labelItem = new QTableWidgetItem(simulation->getLabel());
        QTableWidgetItem *statusItem = new QTableWidgetItem(simulation->getSimulationStatusMap().value(simulation->getStatus()));
        QTableWidgetItem *progressItem = new QTableWidgetItem(QString::number(simulation->getProgress()) + "%");
        
        QObject::connect(simulation, SIGNAL(updateProgress(int)), this, SLOT(onUpdateSimulationProgress(int)));
        QObject::connect(simulation, SIGNAL(updateStatus(SimulationStatus)), this, SLOT(onUpdateSimulationStatus(SimulationStatus)));
        
		ui->tblAll->insertRow(row);
		ui->tblAll->setItem(row, 0, labelItem);
        ui->tblAll->setItem(row, 1, statusItem);
		ui->tblAll->setItem(row, 2, progressItem);
        
        if (simulation->getStatus() == SimulationStatus::IDLE) {
            row = ui->tblIdle->rowCount();
            ui->tblIdle->insertRow(row);
            ui->tblIdle->setItem(row, 0, labelItem->clone());
            ui->tblIdle->setItem(row, 1, progressItem->clone());
        } else if (simulation->getStatus() == SimulationStatus::RUNNING) {
            row = ui->tblRunning->rowCount();
            ui->tblRunning->insertRow(row);
            ui->tblRunning->setItem(row, 0, labelItem->clone());
            ui->tblRunning->setItem(row, 1, progressItem->clone());
        } else if (simulation->getStatus() == SimulationStatus::PAUSED) {
            row = ui->tblPaused->rowCount();
            ui->tblPaused->insertRow(row);
            ui->tblPaused->setItem(row, 0, labelItem->clone());
            ui->tblPaused->setItem(row, 1, progressItem->clone());
        } else if (simulation->getStatus() == SimulationStatus::FINISHED) {
            row = ui->tblFinished->rowCount();
            ui->tblFinished->insertRow(row);
            ui->tblFinished->setItem(row, 0, labelItem->clone());
            ui->tblFinished->setItem(row, 1, progressItem->clone());
        }
	}
}

Simulation* SimulationManagerDialog::getCurrentSimulation() const {
    QTableWidget *tableWidget = nullptr;
    QString currentTabName = ui->tabWidget->currentWidget()->objectName();
    
    if (currentTabName == "tabAll") {
        tableWidget = ui->tblAll;
    } else if (currentTabName == "tabIdle") {
        tableWidget = ui->tblIdle;
    } else if (currentTabName == "tabPaused") {
        tableWidget = ui->tblPaused;
    } else if (currentTabName == "tabRunning") {
        tableWidget = ui->tblRunning;
    } else {
        tableWidget = ui->tblFinished;
    }
    
    int row = tableWidget->currentRow();
    QTableWidgetItem *labelItem = tableWidget->item(row, 0);
    
    if (labelItem) {
        return IPHApplication::getCurrentProject()->getSimulation(labelItem->text());
    }
    
    return nullptr;
}

QTableWidget* SimulationManagerDialog::getTableWidgetBySimulationStatus(const SimulationStatus &status) {
    if (status == SimulationStatus::IDLE) {
        return ui->tblIdle;
    } else if (status == SimulationStatus::PAUSED) {
        return ui->tblPaused;
    } else if (status == SimulationStatus::RUNNING) {
        return ui->tblRunning;
    } else if (status == SimulationStatus::FINISHED) {
        return ui->tblFinished;
    }
    
    return ui->tblAll;
}

void SimulationManagerDialog::on_tblAll_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    if (current) {
        QTableWidgetItem *statusItem = ui->tblAll->item(current->row(), 1);
        SimulationStatus status = Simulation::getSimulationStatusMap().key(statusItem->text());

        ui->btnResume->setEnabled(status == SimulationStatus::IDLE || status == SimulationStatus::PAUSED);
        ui->btnPause->setEnabled(status == SimulationStatus::RUNNING);
        ui->btnRemove->setEnabled(true);
        ui->btnFinish->setEnabled(status != SimulationStatus::IDLE && status != SimulationStatus::FINISHED);
    } else {
        ui->btnRemove->setEnabled(false);
        ui->btnFinish->setEnabled(false);
    }
}

void SimulationManagerDialog::on_tblIdle_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    ui->btnResume->setEnabled(true);
    ui->btnPause->setEnabled(false);
    ui->btnRemove->setEnabled(true);
    ui->btnFinish->setEnabled(true);
}

void SimulationManagerDialog::on_tblRunning_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    ui->btnResume->setEnabled(false);
    ui->btnPause->setEnabled(true);
    ui->btnRemove->setEnabled(true);
    ui->btnFinish->setEnabled(true);
}

void SimulationManagerDialog::on_tblPaused_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    ui->btnResume->setEnabled(true);
    ui->btnPause->setEnabled(false);
    ui->btnRemove->setEnabled(true);
    ui->btnFinish->setEnabled(true);
}

void SimulationManagerDialog::on_tblFinished_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    ui->btnResume->setEnabled(false);
    ui->btnPause->setEnabled(false);
    ui->btnRemove->setEnabled(true);
    ui->btnFinish->setEnabled(false);
}

void SimulationManagerDialog::onUpdateSimulationProgress(int progress) {
    Simulation *simulation = qobject_cast<Simulation*>(sender());
    
    for (int row = 0; row < ui->tblAll->rowCount(); row++) {
        QTableWidgetItem *labelItem = ui->tblAll->item(row, 0);
        
        if (labelItem->text() == simulation->getLabel()) {
            QTableWidgetItem *progressItem = ui->tblAll->item(row, 2);
            progressItem->setText(QString::number(simulation->getProgress()) + "%");
            break;
        }
    }
    
    QTableWidget *currentTabWidget = this->getTableWidgetBySimulationStatus(simulation->getStatus());
    
    if (currentTabWidget && currentTabWidget != ui->tblFinished) {
        for (int row = 0; row < currentTabWidget->rowCount(); row++) {
            QTableWidgetItem *labelItem = currentTabWidget->item(row, 0);
            
            if (labelItem->text() == simulation->getLabel()) {
                QTableWidgetItem *progressItem = currentTabWidget->item(row, 1);
                progressItem->setText(QString::number(simulation->getProgress()) + "%");
                break;
            }
        }
    }
}

void SimulationManagerDialog::onUpdateSimulationStatus(SimulationStatus status) {
    Simulation *simulation = qobject_cast<Simulation*>(sender());
    
    for (int row = 0; row < ui->tblAll->rowCount(); row++) {
        QTableWidgetItem *labelItem = ui->tblAll->item(row, 0);
        
        if (labelItem->text() == simulation->getLabel()) {
            QTableWidgetItem *statusItem = ui->tblAll->item(row, 1);
            statusItem->setText(Simulation::getSimulationStatusMap().value(simulation->getStatus()));
            ui->btnResume->setEnabled(simulation->getStatus() == SimulationStatus::IDLE || simulation->getStatus() == SimulationStatus::PAUSED);
            ui->btnPause->setEnabled(simulation->getStatus() == SimulationStatus::RUNNING);
            ui->btnFinish->setEnabled(status != SimulationStatus::IDLE && status != SimulationStatus::FINISHED);
            break;
        }
    }
    
    QTableWidget *previousTableWidget = this->getTableWidgetBySimulationStatus(simulation->getPreviousStatus());
    
    if (previousTableWidget) {
        for (int row = 0; row < previousTableWidget->rowCount(); row++) {
            QTableWidgetItem *labelItem = previousTableWidget->item(row, 0);
            
            if (labelItem->text() == simulation->getLabel()) {
                previousTableWidget->removeRow(row);
                // TODO:
//                ui->btnResume->setEnabled(false);
//                ui->btnPause->setEnabled(false);
                break;
            }
        }
    }
    
    QTableWidget *currentTabWidget = this->getTableWidgetBySimulationStatus(simulation->getStatus());
    
    if (currentTabWidget) {
        int rowCount = currentTabWidget->rowCount();

        currentTabWidget->insertRow(rowCount);
        currentTabWidget->setItem(rowCount, 0, new QTableWidgetItem(simulation->getLabel()));
        
        if (currentTabWidget != ui->tblFinished) {
            currentTabWidget->setItem(rowCount, 1, new QTableWidgetItem(QString::number(simulation->getProgress()) + "%"));
            currentTabWidget->setItem(rowCount, 2, new QTableWidgetItem(Simulation::getSimulationStatusMap().value(simulation->getStatus())));
        }
    }
}

void SimulationManagerDialog::onSimulationCreated(Simulation *simulation) {
    int rowCount = ui->tblAll->rowCount();
    
    ui->tblAll->insertRow(rowCount);
    ui->tblAll->setItem(rowCount, 0, new QTableWidgetItem(simulation->getLabel()));
    ui->tblAll->setItem(rowCount, 1, new QTableWidgetItem(Simulation::getSimulationStatusMap().value(simulation->getStatus())));
    ui->tblAll->setItem(rowCount, 2, new QTableWidgetItem(QString::number(simulation->getProgress()) + "%"));
    
    QTableWidget *tableWidget = this->getTableWidgetBySimulationStatus(simulation->getStatus());
    rowCount = tableWidget->rowCount();
    
    tableWidget->insertRow(rowCount);
    tableWidget->setItem(rowCount, 0, new QTableWidgetItem(simulation->getLabel()));
    tableWidget->setItem(rowCount, 1, new QTableWidgetItem(QString::number(simulation->getProgress()) + "%"));
    
    QObject::connect(simulation, SIGNAL(updateProgress(int)), this, SLOT(onUpdateSimulationProgress(int)));
    QObject::connect(simulation, SIGNAL(updateStatus(SimulationStatus)), this, SLOT(onUpdateSimulationStatus(SimulationStatus)));
}

void SimulationManagerDialog::on_btnResume_clicked() {
    Simulation *simulation = this->getCurrentSimulation();
    
    if (simulation) {
        SimulationManager *simulationManager = SimulationManager::getInstance();
        simulationManager->resume(simulation);
    }
}

void SimulationManagerDialog::on_btnFinish_clicked() {
    Simulation *simulation = this->getCurrentSimulation();
    
    if (simulation) { // selected row may be empty
        QString question = tr("Are you sure you want to finish the selected simulation?");
        QMessageBox::StandardButton button = QMessageBox::question(this, tr("Simulation Manager"), question);
        
        if (button == QMessageBox::Yes) {
            SimulationManager *simulationManager = SimulationManager::getInstance();
            simulationManager->finish(simulation);
        }
    }
}

void SimulationManagerDialog::on_btnPause_clicked() {
    Simulation *simulation = this->getCurrentSimulation();
    
    if (simulation) {
        SimulationManager *simulationManager = SimulationManager::getInstance();
        simulationManager->pause(simulation);
    }
}

void SimulationManagerDialog::on_btnRemove_clicked() {
    Simulation *simulation = this->getCurrentSimulation();
    
    if (simulation) {
        QString question = tr("Are you sure you want to remove the selected simulation?");
        QMessageBox::StandardButton button = QMessageBox::question(this, tr("Simulation Manager"), question);
        
        if (button == QMessageBox::Yes) {
            SimulationManager *simulationManager = SimulationManager::getInstance();
            QTableWidget *tableWidget = this->getTableWidgetBySimulationStatus(simulation->getStatus());
            
            for (int row = 0; row < ui->tblAll->rowCount(); row++) {
                QTableWidgetItem *labelItem = ui->tblAll->item(row, 0);
                
                if (labelItem->text() == simulation->getLabel()) {
                    ui->tblAll->removeRow(row);
                    break;
                }
            }
            
            for (int row = 0; row < tableWidget->rowCount(); row++) {
                QTableWidgetItem *labelItem = tableWidget->item(row, 0);
                
                if (labelItem->text() == simulation->getLabel()) {
                    tableWidget->removeRow(row);
                    break;
                }
            }
            
            simulationManager->remove(simulation);
        }
    }
}

void SimulationManagerDialog::on_btnClose_clicked() {
    MainWindow *mainWindow = static_cast<MainWindow*>(this->topLevelWidget());
    
    for (QAction *action : toolBarActions) {
        mainWindow->getToolBar()->removeAction(action);
    }
    
    QMdiSubWindow *parentWindow = static_cast<QMdiSubWindow*>(parent());
    parentWindow->close();
}
