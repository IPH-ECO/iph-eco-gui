#include "include/ui/simulation_manager_dialog.h"
#include "ui_simulation_manager_dialog.h"

#include "include/application/iph_application.h"
#include "include/exceptions/simulation_exception.h"
#include "include/application/simulation_manager.h"
#include "include/repository/simulation_repository.h"
#include "include/ui/main_window.h"
#include "include/ui/layer_properties_dialog.h"

#include <QDir>
#include <QRegExp>
#include <QAction>
#include <QToolButton>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>

SimulationManagerDialog::SimulationManagerDialog(QWidget *parent) : AbstractMeshDialog(parent), ui(new Ui::SimulationManagerDialog) {
	ui->setupUi(this);
    ui->tblAll->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblIdle->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblRunning->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblPaused->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblFinished->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblLayers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    this->vtkWidget = ui->vtkWidget;

	Project *project = IPHApplication::getCurrentProject();
    
    QObject::connect(project, SIGNAL(simulationCreated(Simulation*)), this, SLOT(onSimulationCreated(Simulation*)));
    QObject::connect(&frameTimer, SIGNAL(timeout()), this, SLOT(renderNextFrame()));

    ui->tblAll->blockSignals(true);
    ui->tblIdle->blockSignals(true);
    ui->tblRunning->blockSignals(true);
    ui->tblPaused->blockSignals(true);
    ui->tblFinished->blockSignals(true);
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
    ui->tblAll->blockSignals(false);
    ui->tblIdle->blockSignals(false);
    ui->tblRunning->blockSignals(false);
    ui->tblPaused->blockSignals(false);
    ui->tblFinished->blockSignals(false);
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
        Simulation *simulation = this->getCurrentSimulation();
        QFileInfoList outputFiles = simulation->getOutputFiles();
        
        ui->btnResume->setEnabled(status == SimulationStatus::IDLE || status == SimulationStatus::PAUSED);
        ui->btnPause->setEnabled(status == SimulationStatus::RUNNING);
        ui->btnRemove->setEnabled(true);
        ui->btnFinish->setEnabled(status != SimulationStatus::IDLE && status != SimulationStatus::FINISHED);
        ui->btnRefresh->setEnabled(true);
        ui->spxFrame->setValue(1);
        ui->spxFrame->setMaximum(outputFiles.size());
        ui->lblFrameTotal->setText(QString::number(outputFiles.size()));
        
        fillLayersComboBox(simulation);
        
        if (previous) {
            ui->vtkWidget->render(simulation, "", "", 0); // Only renders the mesh
        }
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

void SimulationManagerDialog::on_btnRefresh_clicked() {
    Simulation *simulation = this->getCurrentSimulation();
    
    if (simulation) {
        QFileInfoList outputFiles = simulation->getOutputFiles();
        
        ui->spxFrame->setMaximum(outputFiles.size());
        ui->lblFrameTotal->setText(QString::number(outputFiles.size()));
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

void SimulationManagerDialog::on_btnFirstFrame_clicked() {
    ui->spxFrame->setValue(1);
}

void SimulationManagerDialog::on_btnPreviousFrame_clicked() {
    int frame = ui->spxFrame->value();
    
    if (frame > 1) {
        ui->spxFrame->setValue(ui->spxFrame->value() - 1);
    }
}

void SimulationManagerDialog::on_btnStartReproduction_clicked() {
    ui->btnStartReproduction->setEnabled(false);
    ui->btnPauseReproduction->setEnabled(true);
    frameTimer.start(500);
}

void SimulationManagerDialog::on_btnPauseReproduction_clicked() {
    frameTimer.stop();
    ui->btnPauseReproduction->setEnabled(false);
    ui->btnStartReproduction->setEnabled(true);
}

void SimulationManagerDialog::on_btnNextFrame_clicked() {
    Simulation *simulation = this->getCurrentSimulation();
    
    if (simulation) {
        QFileInfoList outputFiles = simulation->getOutputFiles();
        int frame = ui->spxFrame->value();
        
        if (frame != outputFiles.size()) {
            ui->spxFrame->setValue(ui->spxFrame->value() + 1);
        }
    }
}

void SimulationManagerDialog::on_btnLastFrame_clicked() {
    Simulation *simulation = this->getCurrentSimulation();
    
    if (simulation) {
        QFileInfoList outputFiles = simulation->getOutputFiles();
        ui->spxFrame->setValue(outputFiles.size());
    }
}

void SimulationManagerDialog::on_btnAddLayer_clicked() {
    QString layer = ui->cbxLayers->currentText();
    
    if (!layer.isEmpty()) {
        QList<QTableWidgetItem*> foundItems = ui->tblLayers->findItems(layer, Qt::MatchExactly);
        
        if (foundItems.isEmpty()) {
            int row = ui->tblLayers->rowCount();
            QTableWidgetItem *layerItem = new QTableWidgetItem(layer);
            
            layerItem->setData(Qt::UserRole, ui->cbxLayers->currentData());

            ui->tblLayers->insertRow(row);
            ui->tblLayers->setItem(row, 0, layerItem);
            
            QWidget *itemWidget = new QWidget();
            QHBoxLayout *hLayout = new QHBoxLayout(itemWidget);
            QToolButton *showHideLayerButton = new QToolButton();
            QIcon hiddenIcon(":/icons/layer-visible-on.png");
            
            showHideLayerButton->setIcon(hiddenIcon);
            showHideLayerButton->setToolTip("Show/Hide layer");
            showHideLayerButton->setCheckable(true);
            showHideLayerButton->setObjectName(QString("showHideLayerButton-%1").arg(row));
            
            hLayout->addWidget(showHideLayerButton);
            hLayout->setAlignment(Qt::AlignCenter);
            hLayout->setContentsMargins(0, 0, 0, 0);
            
            QToolButton *layerPropertiesButton = new QToolButton();
            QIcon layerPropertiesIcon(":/icons/interpolation-result.png");
            
            layerPropertiesButton->setIcon(layerPropertiesIcon);
            layerPropertiesButton->setToolTip("Layer properties");
            layerPropertiesButton->setObjectName(QString("layerPropertiesButton-%1").arg(row));
            
            hLayout->addWidget(layerPropertiesButton);
            
            QToolButton *removeLayerButton = new QToolButton();
            QIcon removeIcon(":/icons/list-delete.png");
            
            removeLayerButton->setIcon(removeIcon);
            removeLayerButton->setToolTip("Remove layer");
            removeLayerButton->setObjectName(QString("removeLayerButton-%1").arg(row));
            
            hLayout->addWidget(removeLayerButton);
            hLayout->addStretch();
            
            itemWidget->setLayout(hLayout);
            ui->tblLayers->setCellWidget(row, 1, itemWidget);
            
            QObject::connect(showHideLayerButton, SIGNAL(clicked(bool)), this, SLOT(toggleLayerVisibility(bool)));
            QObject::connect(layerPropertiesButton, SIGNAL(clicked()), this, SLOT(editLayerProperties()));
            QObject::connect(removeLayerButton, SIGNAL(clicked()), this, SLOT(removeLayer()));
            
            Simulation *simulation = getCurrentSimulation();
            
            simulation->addSelectedLayer(ui->cbxLayers->currentData().toString());
        } else {
            QMessageBox::warning(this, tr("Simulation Manager"), tr("Layer already added"));
        }
    }
}

void SimulationManagerDialog::fillLayersComboBox(Simulation *simulation) {
    QStringList outputParameters = simulation->getOutputParameters();
    QMap<QString, QString> layers = SimulationRepository::loadOutputParametersLabels(outputParameters);
    int i = 0;
    
    ui->cbxLayers->clear();
    for (QString layerLabel : layers.keys()) {
        ui->cbxLayers->addItem(layerLabel);
        ui->cbxLayers->setItemData(i, layers.value(layerLabel));
        i++;
    }
    ui->cbxLayers->setCurrentIndex(-1);
}

void SimulationManagerDialog::removeLayer() {
    QString question = tr("Are you sure you want to remove the selected layer from the list?");
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("Simulation Manager"), question);
    int row = static_cast<QToolButton*>(sender())->objectName().split("-")[1].toInt();
 
    if (button == QMessageBox::Yes) {
        frameTimer.stop();
        ui->vtkWidget->clear();
        ui->tblLayers->removeRow(row);
    }
}

void SimulationManagerDialog::toggleLayerVisibility(bool show) {
    if (show) {
        int row = static_cast<QToolButton*>(sender())->objectName().split("-")[1].toInt();
        QTableWidgetItem *layerItem = ui->tblLayers->item(row, 0);
        QStringList layerAndComponent = layerItem->data(Qt::UserRole).toString().split("-");
        Simulation *simulation = getCurrentSimulation();
        
        ui->vtkWidget->render(simulation, layerAndComponent.first(), layerAndComponent.last(), ui->spxFrame->value() - 1);
    } else {
        frameTimer.stop();
        ui->vtkWidget->clear();
    }
}

void SimulationManagerDialog::renderNextFrame() {
    if (ui->spxFrame->value() == ui->spxFrame->maximum()) {
        if (ui->btnLoop->isChecked()) {
            ui->spxFrame->setValue(1);
        } else {
            on_btnPauseReproduction_clicked();
        }
    } else {
        ui->spxFrame->setValue(ui->spxFrame->value() + 1);
    }
}

void SimulationManagerDialog::on_spxFrame_valueChanged(int frame) {
    for (QToolButton *button : ui->tblLayers->findChildren<QToolButton*>(QRegExp("^showHideLayerButton"))) {
        if (button->isChecked()) {
            int row = button->objectName().split("-")[1].toInt();
            QTableWidgetItem *layerItem = ui->tblLayers->item(row, 0);
            QStringList layerAndComponent = layerItem->data(Qt::UserRole).toString().split("-");
            Simulation *simulation = getCurrentSimulation();
            
            ui->vtkWidget->render(simulation, layerAndComponent.first(), layerAndComponent.last(), ui->spxFrame->value() - 1);
            break;
        }
    }
}

void SimulationManagerDialog::editLayerProperties() {
    int row = static_cast<QToolButton*>(sender())->objectName().split("-")[1].toInt();
    QTableWidgetItem *layerItem = ui->tblLayers->item(row, 0);
    Simulation *simulation = getCurrentSimulation();
    LayerProperties *layerProperties = simulation->getSelectedLayers().value(layerItem->data(Qt::UserRole).toString());
    LayerPropertiesDialog *dialog = new LayerPropertiesDialog(this, layerProperties);
    
    dialog->removeMeshTab();
    QObject::connect(dialog, SIGNAL(applyChanges()), ui->vtkWidget, SLOT(updateLayer()));
    dialog->exec();
}