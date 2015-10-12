#include "include/ui/simulation_manager_dialog.h"
#include "ui_simulation_manager_dialog.h"

#include "include/application/iph_application.h"
#include "include/domain/simulation.h"
#include "include/ui/main_window.h"

#include <QMdiSubWindow>

SimulationManagerDialog::SimulationManagerDialog(QWidget *parent) : QWidget(parent), ui(new Ui::SimulationManagerDialog) {
	ui->setupUi(this);
    ui->tblAll->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblIdle->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblRunning->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblPaused->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblFinished->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
	Project *project = IPHApplication::getCurrentProject();

	for (Simulation *simulation : project->getSimulations()) {
		int row = ui->tblAll->rowCount();
        QTableWidgetItem *labelItem = new QTableWidgetItem(simulation->getLabel());
        QTableWidgetItem *statusItem = new QTableWidgetItem(simulation->getSimulationStatusMap().value(simulation->getStatus()));
        QTableWidgetItem *progressItem = new QTableWidgetItem(QString::number(simulation->getProgress()) + "%");
        
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

void SimulationManagerDialog::on_btnClose_clicked() {
    MainWindow *mainWindow = static_cast<MainWindow*>(this->topLevelWidget());
    
    for (QAction *action : toolBarActions) {
        mainWindow->getToolBar()->removeAction(action);
    }
    
    QMdiSubWindow *parentWindow = static_cast<QMdiSubWindow*>(parent());
    parentWindow->close();
}
