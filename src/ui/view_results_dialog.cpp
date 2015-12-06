#include "include/ui/view_results_dialog.h"
#include "ui_view_results_dialog.h"

#include "include/application/iph_application.h"
#include "include/repository/simulation_repository.h"
#include "include/ui/main_window.h"
#include "include/ui/layer_properties_dialog.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QMdiSubWindow>
#include <QRegularExpression>

ViewResultsDialog::ViewResultsDialog(QWidget *parent) :
    AbstractMeshDialog(parent),
    ui(new Ui::ViewResultsDialog),
    currentSimulation(nullptr),
    axesScale("1 1 1")
{
	ui->setupUi(this);
    ui->tblSimulations->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblLayers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->vtkWidget = ui->vtkWidget;
    
    Project *project = IPHApplication::getCurrentProject();
    
    QObject::connect(project, SIGNAL(simulationCreated(Simulation*)), this, SLOT(onSimulationCreated(Simulation*)));
    QObject::connect(&frameTimer, SIGNAL(timeout()), this, SLOT(renderNextFrame()));
    
    ui->tblSimulations->blockSignals(true);
    for (Simulation *simulation : project->getSimulations()) {
        int row = ui->tblSimulations->rowCount();
        QTableWidgetItem *labelItem = new QTableWidgetItem(simulation->getLabel());
        QTableWidgetItem *statusItem = new QTableWidgetItem(simulation->getSimulationStatusMap().value(simulation->getStatus()));
        QTableWidgetItem *progressItem = new QTableWidgetItem(QString::number(simulation->getProgress()) + "%");
        
        QObject::connect(simulation, SIGNAL(updateProgress(int)), this, SLOT(onUpdateSimulationProgress(int)));
        QObject::connect(simulation, SIGNAL(updateStatus(SimulationStatus)), this, SLOT(onUpdateSimulationStatus(SimulationStatus)));
        
        ui->tblSimulations->insertRow(row);
        ui->tblSimulations->setItem(row, 0, labelItem);
        ui->tblSimulations->setItem(row, 1, statusItem);
        ui->tblSimulations->setItem(row, 2, progressItem);
    }
    ui->tblSimulations->blockSignals(false);
}

void ViewResultsDialog::on_tblSimulations_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    if (current) {
        QTableWidgetItem *labelItem = ui->tblSimulations->item(current->row(), 0);
        
        this->currentSimulation = IPHApplication::getCurrentProject()->getSimulation(labelItem->text());
        
        QFileInfoList outputFiles = this->currentSimulation->getOutputFiles();
        
        ui->btnRefresh->setEnabled(true);
        ui->spxFrame->setValue(1);
        ui->spxFrame->setMaximum(outputFiles.size());
        ui->lblFrameTotal->setText(QString::number(outputFiles.size()));
        
        fillLayersComboBox();
        
        if (previous) {
            ui->vtkWidget->render(this->currentSimulation, "", "", 0); // Only renders the mesh
        }
    }
}

void ViewResultsDialog::onUpdateSimulationProgress(int progress) {
    Simulation *simulation = qobject_cast<Simulation*>(sender());
    
    for (int row = 0; row < ui->tblSimulations->rowCount(); row++) {
        QTableWidgetItem *labelItem = ui->tblSimulations->item(row, 0);
        
        if (labelItem->text() == simulation->getLabel()) {
            QTableWidgetItem *progressItem = ui->tblSimulations->item(row, 2);
            progressItem->setText(QString::number(simulation->getProgress()) + "%");
            break;
        }
    }
    
    for (int row = 0; row < ui->tblSimulations->rowCount(); row++) {
        QTableWidgetItem *labelItem = ui->tblSimulations->item(row, 0);
        
        if (labelItem->text() == simulation->getLabel()) {
            QTableWidgetItem *progressItem = ui->tblSimulations->item(row, 1);
            progressItem->setText(QString::number(simulation->getProgress()) + "%");
            break;
        }
    }
}

void ViewResultsDialog::onUpdateSimulationStatus(SimulationStatus status) {
    Simulation *simulation = qobject_cast<Simulation*>(sender());
    
    for (int row = 0; row < ui->tblSimulations->rowCount(); row++) {
        QTableWidgetItem *labelItem = ui->tblSimulations->item(row, 0);
        
        if (labelItem->text() == simulation->getLabel()) {
            QTableWidgetItem *statusItem = ui->tblSimulations->item(row, 1);
            statusItem->setText(Simulation::getSimulationStatusMap().value(simulation->getStatus()));
            break;
        }
    }

    int rowCount = ui->tblSimulations->rowCount();

    ui->tblSimulations->insertRow(rowCount);
    ui->tblSimulations->setItem(rowCount, 0, new QTableWidgetItem(simulation->getLabel()));
    ui->tblSimulations->setItem(rowCount, 1, new QTableWidgetItem(QString::number(simulation->getProgress()) + "%"));
    ui->tblSimulations->setItem(rowCount, 2, new QTableWidgetItem(Simulation::getSimulationStatusMap().value(simulation->getStatus())));
}

void ViewResultsDialog::onSimulationCreated(Simulation *simulation) {
    int rowCount = ui->tblSimulations->rowCount();
    
    ui->tblSimulations->insertRow(rowCount);
    ui->tblSimulations->setItem(rowCount, 0, new QTableWidgetItem(simulation->getLabel()));
    ui->tblSimulations->setItem(rowCount, 1, new QTableWidgetItem(Simulation::getSimulationStatusMap().value(simulation->getStatus())));
    ui->tblSimulations->setItem(rowCount, 2, new QTableWidgetItem(QString::number(simulation->getProgress()) + "%"));
    
    rowCount = ui->tblSimulations->rowCount();
    
    ui->tblSimulations->insertRow(rowCount);
    ui->tblSimulations->setItem(rowCount, 0, new QTableWidgetItem(simulation->getLabel()));
    ui->tblSimulations->setItem(rowCount, 1, new QTableWidgetItem(QString::number(simulation->getProgress()) + "%"));
    
    QObject::connect(simulation, SIGNAL(updateProgress(int)), this, SLOT(onUpdateSimulationProgress(int)));
    QObject::connect(simulation, SIGNAL(updateStatus(SimulationStatus)), this, SLOT(onUpdateSimulationStatus(SimulationStatus)));
}

void ViewResultsDialog::on_btnRefresh_clicked() {
    if (this->currentSimulation) {
        QFileInfoList outputFiles = this->currentSimulation->getOutputFiles();
        
        ui->spxFrame->setMaximum(outputFiles.size());
        ui->lblFrameTotal->setText(QString::number(outputFiles.size()));
    }
}

void ViewResultsDialog::on_btnClose_clicked() {
    MainWindow *mainWindow = static_cast<MainWindow*>(this->topLevelWidget());
    
    for (QAction *action : toolBarActions) {
        mainWindow->getToolBar()->removeAction(action);
    }
    
    QMdiSubWindow *parentWindow = static_cast<QMdiSubWindow*>(parent());
    parentWindow->close();
}

void ViewResultsDialog::on_btnFirstFrame_clicked() {
    ui->spxFrame->setValue(1);
}

void ViewResultsDialog::on_btnPreviousFrame_clicked() {
    int frame = ui->spxFrame->value();
    
    if (frame > 1) {
        ui->spxFrame->setValue(ui->spxFrame->value() - 1);
    }
}

void ViewResultsDialog::on_btnStartReproduction_clicked() {
    ui->btnStartReproduction->setEnabled(false);
    ui->btnPauseReproduction->setEnabled(true);
    frameTimer.start(500);
}

void ViewResultsDialog::on_btnPauseReproduction_clicked() {
    frameTimer.stop();
    ui->btnPauseReproduction->setEnabled(false);
    ui->btnStartReproduction->setEnabled(true);
}

void ViewResultsDialog::on_btnNextFrame_clicked() {
    if (this->currentSimulation) {
        QFileInfoList outputFiles = this->currentSimulation->getOutputFiles();
        int frame = ui->spxFrame->value();
        
        if (frame != outputFiles.size()) {
            ui->spxFrame->setValue(ui->spxFrame->value() + 1);
        }
    }
}

void ViewResultsDialog::on_btnLastFrame_clicked() {
    if (this->currentSimulation) {
        QFileInfoList outputFiles = this->currentSimulation->getOutputFiles();
        ui->spxFrame->setValue(outputFiles.size());
    }
}

void ViewResultsDialog::on_btnAddLayer_clicked() {
    QString layer = ui->cbxLayers->currentText();
    
    if (!layer.isEmpty()) {
        QList<QTableWidgetItem*> foundItems = ui->tblLayers->findItems(layer, Qt::MatchExactly);
        
        if (foundItems.isEmpty()) {
            QString layerKey = ui->cbxLayers->currentData().toString();
            int row = ui->tblLayers->rowCount();
            QTableWidgetItem *layerItem = new QTableWidgetItem(layer);
            
            layerItem->setData(Qt::UserRole, layerKey); //

            ui->tblLayers->insertRow(row);
            ui->tblLayers->setItem(row, 0, layerItem);
            
            QWidget *itemWidget = new QWidget();
            QHBoxLayout *hLayout = new QHBoxLayout(itemWidget);
            QToolButton *showHideLayerButton = new QToolButton();
            QIcon hiddenIcon(":/icons/layer-visible-on.png");
            
            showHideLayerButton->setIcon(hiddenIcon);
            showHideLayerButton->setToolTip("Show/Hide layer");
            showHideLayerButton->setCheckable(true);
            showHideLayerButton->setObjectName(QString("showHideLayerButton-%1").arg(layerKey));
            
            hLayout->addWidget(showHideLayerButton);
            hLayout->setAlignment(Qt::AlignCenter);
            hLayout->setContentsMargins(0, 0, 0, 0);
            
            QToolButton *layerPropertiesButton = new QToolButton();
            QIcon layerPropertiesIcon(":/icons/interpolation-result.png");
            
            layerPropertiesButton->setIcon(layerPropertiesIcon);
            layerPropertiesButton->setToolTip("Layer properties");
            layerPropertiesButton->setObjectName(QString("layerPropertiesButton-%1").arg(layerKey));
            
            hLayout->addWidget(layerPropertiesButton);
            
            QToolButton *removeLayerButton = new QToolButton();
            QIcon removeIcon(":/icons/list-delete.png");
            
            removeLayerButton->setIcon(removeIcon);
            removeLayerButton->setToolTip("Remove layer");
            removeLayerButton->setObjectName(QString("removeLayerButton-%1").arg(layerKey));
            
            hLayout->addWidget(removeLayerButton);
            hLayout->addStretch();
            
            itemWidget->setLayout(hLayout);
            ui->tblLayers->setCellWidget(row, 1, itemWidget);
            
            QObject::connect(showHideLayerButton, SIGNAL(clicked(bool)), this, SLOT(toggleLayerVisibility(bool)));
            QObject::connect(layerPropertiesButton, SIGNAL(clicked()), this, SLOT(editLayerProperties()));
            QObject::connect(removeLayerButton, SIGNAL(clicked()), this, SLOT(removeLayer()));
            
            this->currentSimulation->addSelectedLayer(ui->cbxLayers->currentData().toString());
        } else {
            QMessageBox::warning(this, tr("View Results"), tr("Layer already added"));
        }
    }
}

void ViewResultsDialog::fillLayersComboBox() {
    QStringList outputParameters = this->currentSimulation->getOutputParameters();
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

void ViewResultsDialog::removeLayer() {
    QString question = tr("Are you sure you want to remove the selected layer from the list?");
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("View Results"), question);
    
    if (button == QMessageBox::Yes) {
        QString layerKey = getLayerKeyFromButton(static_cast<QToolButton*>(sender()));
        
        ui->vtkWidget->removeLayer(layerKey);
        
        for (int row = 0; row < ui->tblLayers->rowCount(); row++) {
            QTableWidgetItem *item = ui->tblLayers->item(row, 0);

            if (item->data(Qt::UserRole).toString() == layerKey) {
                ui->tblLayers->removeRow(row);
                break;
            }
        }
    }
}

void ViewResultsDialog::toggleLayerVisibility(bool show) {
    QToolButton *showHideLayerButton = static_cast<QToolButton*>(sender());
    QString layerKey = getLayerKeyFromButton(showHideLayerButton);
    
    if (show) {
        QStringList layerAndComponent = layerKey.split("-");
        
        if (layerAndComponent.last() != "Vector") {
            for (QToolButton *toolButton : ui->tblLayers->findChildren<QToolButton*>(QRegExp("^showHideLayerButton"))) {
                if (toolButton != showHideLayerButton && toolButton->isChecked()) {
                    QString buttonLayerKey = getLayerKeyFromButton(toolButton);
                    
                    if (buttonLayerKey.split("-").last() != "Vector") {
                        toolButton->setChecked(false);
                    }
                }
            }
        }
        
        ui->vtkWidget->render(this->currentSimulation, layerAndComponent.first(), layerAndComponent.last(), ui->spxFrame->value() - 1);
    } else {
        ui->vtkWidget->hideLayer(layerKey);
    }
}

QString ViewResultsDialog::getLayerKeyFromButton(QToolButton *button) const {
    QString buttonName = button->objectName();
    return buttonName.remove(buttonName.split("-").first() + "-");
}

void ViewResultsDialog::renderNextFrame() {
    if (ui->spxFrame->value() == ui->spxFrame->maximum()) {
        ui->spxFrame->setValue(1);
        if (!ui->btnLoop->isChecked()) {
            on_btnPauseReproduction_clicked();
        }
    } else {
        ui->spxFrame->setValue(ui->spxFrame->value() + 1);
    }
}

void ViewResultsDialog::on_spxFrame_valueChanged(int frame) {
    if (this->currentSimulation) {
        for (QToolButton *button : ui->tblLayers->findChildren<QToolButton*>(QRegExp("^showHideLayerButton"))) {
            if (button->isChecked()) {
                QStringList layerAndComponent = getLayerKeyFromButton(button).split("-");
                ui->vtkWidget->render(this->currentSimulation, layerAndComponent.first(), layerAndComponent.last(), ui->spxFrame->value() - 1);
            }
        }
    }
}

void ViewResultsDialog::editLayerProperties() {
    QString layerKey = getLayerKeyFromButton(static_cast<QToolButton*>(sender()));
    QStringList layerAndComponent = layerKey.split("-");
    LayerProperties *layerProperties = this->currentSimulation->getSelectedLayers().value(layerKey);
    LayerPropertiesDialog::LayerPropertiesTab tab = layerAndComponent.last() == "Vector" ? LayerPropertiesDialog::VECTORS : LayerPropertiesDialog::MAP;
    LayerPropertiesDialog *dialog = new LayerPropertiesDialog(this, layerProperties, tab);
    
    QObject::connect(dialog, SIGNAL(applyChanges()), ui->vtkWidget, SLOT(updateLayer()));
    dialog->exec();
}

void ViewResultsDialog::showEvent(QShowEvent *event) {
    if (!event->spontaneous() && this->windowState() & Qt::WindowMaximized) {
        MainWindow *mainWindow = static_cast<MainWindow*>(this->topLevelWidget());
        QAction *separator = mainWindow->getToolBar()->addSeparator();
        toolBarActions.append(separator);
        
        QAction *scaleAxesAction = new QAction(QIcon(":/icons/scale-axes.png"), "Change axes scale", mainWindow);
        QObject::connect(scaleAxesAction, SIGNAL(triggered()), this, SLOT(showAxesDialog()));
        toolBarActions.append(scaleAxesAction);
    }
    
    AbstractMeshDialog::showEvent(event);
}

void ViewResultsDialog::showAxesDialog() {
    QString inputText = QInputDialog::getText(this, "Axes scale", "X Y Z", QLineEdit::Normal, axesScale);
    
    if (!inputText.isEmpty() && axesScale != inputText) {
        QRegularExpression regexp("\\d\\ \\d\\ \\d");
        
        if (!regexp.match(inputText).hasMatch()) {
            QMessageBox::warning(this, tr("View Results"), tr("Invalid scaling string."));
            return;
        }
        
        axesScale = inputText;
        ui->vtkWidget->setAxesScale(axesScale);
    }
}