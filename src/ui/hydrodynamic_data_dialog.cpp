#include <ui/hydrodynamic_data_dialog.h>
#include "ui_hydrodynamic_data_dialog.h"

#include <application/iph_application.h>
#include <domain/unstructured_mesh.h>
#include <domain/structured_mesh.h>
#include <domain/hydrodynamic_parameter.h>
#include <ui/boundary_condition_dialog.h>

#include <QTreeWidgetItemIterator>
#include <QColorDialog>
#include <QFormLayout>
#include <QMessageBox>
#include <QLineEdit>
#include <QPixmap>
#include <QColor>
#include <QSet>

HydrodynamicDataDialog::HydrodynamicDataDialog(QWidget *parent) :
    AbstractMeshDialog(parent), ui(new Ui::HydrodynamicDataDialog), unsavedConfiguration(new HydrodynamicConfiguration), currentConfiguration(unsavedConfiguration),
    currentGridDataConfiguration(nullptr), boundaryConditionDialog(nullptr)
{
    hydrodynamicDataRepository = HydrodynamicDataRepository::getInstance();
    
    ui->setupUi(this);
    ui->trwParameters->header()->setStretchLastSection(false);
    ui->trwParameters->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tblBoundaryConditions->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->vtkWidget = ui->vtkWidget;
    
    Project *project = IPHApplication::getCurrentProject();
    
    ui->cbxConfiguration->blockSignals(true);
    for (HydrodynamicConfiguration *configuration : project->getHydrodynamicConfigurations()) {
        ui->cbxConfiguration->addItem(configuration->getName());
    }
    ui->cbxConfiguration->setCurrentIndex(-1);
    ui->cbxConfiguration->blockSignals(false);

    QSet<GridDataConfiguration*> gridDataConfigurations = project->getGridDataConfigurations();

    ui->cbxGridDataConfiguration->blockSignals(true);
    for (QSet<GridDataConfiguration*>::const_iterator it = gridDataConfigurations.begin(); it != gridDataConfigurations.end(); ++it) {
        ui->cbxGridDataConfiguration->addItem((*it)->getName());
    }
    ui->cbxGridDataConfiguration->setCurrentIndex(-1);
    ui->cbxGridDataConfiguration->blockSignals(false);

    this->setupItems();
}

HydrodynamicDataDialog::~HydrodynamicDataDialog() {
    delete unsavedConfiguration;
    delete ui;
}

void HydrodynamicDataDialog::addParameterItemWidget(HydrodynamicParameter *parameter) {
    if (!parameter->isProcessInput() || parameter->getItemWidget()) {
        return;
    }
    
    QTreeWidgetItem *item = nullptr;
    
    if (parameter->getParent()) {
        item = new QTreeWidgetItem(parameter->getParent()->getItemWidget(), QStringList(parameter->getLabel()));
    } else {
        item = new QTreeWidgetItem(ui->trwParameters, QStringList(parameter->getLabel()));
    }
    
    parameter->setItemWidget(item);
    item->setData(0, Qt::UserRole, QVariant(parameter->getName()));
    
    if (parameter->isEditable()) {
        QLineEdit *lineEdit = new QLineEdit(ui->trwParameters);
        
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setObjectName(parameter->getName());
        lineEdit->setText(QString::number(parameter->getValue()));
        lineEdit->setEnabled(parameter->isEnabled());
        ui->trwParameters->setItemWidget(item, 1, lineEdit);
    }
    
    for (HydrodynamicParameter *child : parameter->getChildren()) {
        addParameterItemWidget(child);
    }
}

void HydrodynamicDataDialog::setupItems() {
    hydrodynamicDataRepository->buildParameters(currentConfiguration);

    QList<HydrodynamicParameter*> parameters = currentConfiguration->getParameters();
    QList<HydrodynamicProcess*> processes = hydrodynamicDataRepository->getProcesses(currentConfiguration);
    QList<HydrodynamicParameter*> rootParameters = currentConfiguration->getRootParameters();
    
    // Parameters
    ui->trwParameters->blockSignals(true);
    for (int i = 0; i < rootParameters.size(); i++) {
        addParameterItemWidget(rootParameters[i]);
    }
    ui->trwParameters->blockSignals(false);
    
    // Processes
    ui->trwProcesses->blockSignals(true);
    for (int i = 0; i < processes.size(); i++) {
        HydrodynamicProcess *process = processes[i];
        QTreeWidgetItem *item = nullptr;
        
        if (!process->getParent()) {
            item = new QTreeWidgetItem(ui->trwProcesses, QStringList(process->getLabel()));
        } else {
            QTreeWidgetItem *parentItem = nullptr;
            QTreeWidgetItemIterator it(ui->trwProcesses);
            
            while (*it) {
                QString itemName = (*it)->data(0, Qt::UserRole).toString();
                if (itemName == process->getParent()->getName()) {
                    parentItem = *it;
                    break;
                }
                it++;
            }
            
            item = new QTreeWidgetItem(parentItem, QStringList(process->getLabel()));
        }
        
        process->setItemWidget(item);
        item->setData(0, Qt::UserRole, QVariant(process->getName()));
    }
    ui->trwProcesses->blockSignals(false);
    
    for (int i = 0; i < processes.size(); i++) {
        if (!processes[i]->isCheckable() || processes[i]->isCheckableGroup()) {
            continue;
        }
        if (processes[i]->isChecked()) {
            on_trwProcesses_itemChanged(processes[i]->getItemWidget(), 0);
        } else {
            processes[i]->getTargetParameter()->toggleSubTreeVisibility(true);
        }
    }
    
    this->expandParameterTree();
    
    // Initial conditions
    for (HydrodynamicParameter *parameter : parameters) {
        if (parameter->isInitialCondition()) {
            QFormLayout *layout = static_cast<QFormLayout*>(ui->initialConditionsTab->layout());
            QLabel *label = new QLabel(parameter->getLabel(), ui->initialConditionsTab);
            QLineEdit *lineEdit = new QLineEdit(ui->initialConditionsTab);
            
            lineEdit->setObjectName(parameter->getName());
            lineEdit->setText(QString::number(parameter->getValue()));
            lineEdit->setAlignment(Qt::AlignRight);
            layout->addRow(label, lineEdit);
        }
    }
}

void HydrodynamicDataDialog::expandParameterTree() {
    QTreeWidgetItemIterator it(ui->trwParameters, QTreeWidgetItemIterator::All);
    
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
}

void HydrodynamicDataDialog::on_cbxConfiguration_currentIndexChanged(const QString &configurationName) {
    clearLayout(ui->initialConditionsTab->layout());
    
    if (!configurationName.isEmpty()) {
        currentConfiguration = IPHApplication::getCurrentProject()->getHydrodynamicConfiguration(configurationName);
        ui->edtConfigurationName->setText(currentConfiguration->getName());
        ui->cbxGridDataConfiguration->setCurrentText(currentConfiguration->getGridDataConfiguration()->getName());
        
        QList<BoundaryCondition*> boundaryConditions = currentConfiguration->getBoundaryConditions();
        int i = 0;
        
        for (BoundaryCondition *boundaryCondition : boundaryConditions) {
            ui->tblBoundaryConditions->insertRow(i);
            ui->tblBoundaryConditions->setItem(i, 0, new QTableWidgetItem(boundaryCondition->getTypeLabel()));
            ui->tblBoundaryConditions->setItem(i, 1, new QTableWidgetItem(boundaryCondition->getFunctionLabel()));
            ui->vtkWidget->getMouseInteractor()->renderBoundaryCondition(boundaryCondition);
            i++;
        }
    }
    
    ui->trwProcesses->clear();
    ui->trwParameters->clear();
    
    this->setupItems();
}

void HydrodynamicDataDialog::on_cbxGridDataConfiguration_currentIndexChanged(const QString &gridDataConfigurationName) {
    ui->btnAddBoundaryCondition->setEnabled(!gridDataConfigurationName.isEmpty());
    
    if (gridDataConfigurationName.isEmpty()) {
        return;
    }
    
    GridDataConfiguration *gridDataConfiguration = currentConfiguration->getGridDataConfiguration();
    
    if (currentGridDataConfiguration && gridDataConfiguration && currentConfiguration->getGridDataConfiguration()->getName() != gridDataConfigurationName && !currentConfiguration->getBoundaryConditions().empty()) {
        QString question = tr("Changing the mesh in this configuration will remove all created boundary conditions. Are you sure?");
        QMessageBox::StandardButton button = QMessageBox::question(this, tr("Hydrodynamic Data"), question);
        
        if (button == QMessageBox::No) {
            ui->cbxGridDataConfiguration->blockSignals(true);
            ui->cbxGridDataConfiguration->setCurrentText(currentGridDataConfiguration->getName());
            ui->cbxGridDataConfiguration->blockSignals(false);
            return;
        }
        
        currentConfiguration->clearBoundaryConditions();
    }
    
    currentGridDataConfiguration = IPHApplication::getCurrentProject()->getGridDataConfiguration(gridDataConfigurationName);
    currentConfiguration->setGridDataConfiguration(currentGridDataConfiguration);
    
    QLineEdit *latitude = this->findChild<QLineEdit*>("latitude");
    latitude->setText(QString::number(currentGridDataConfiguration->getMesh()->getLatitudeAverage()));
    
    ui->vtkWidget->render(currentConfiguration);
}

void HydrodynamicDataDialog::on_btnRemoveConfiguration_clicked() {
    QMessageBox::StandardButton question = QMessageBox::question(this, tr("Hydrodynamic Data"), tr("Are you sure you want to remove the selected configuration?"));
    
    if (question == QMessageBox::Yes) {
        IPHApplication::getCurrentProject()->removeHydrodynamicConfiguration(ui->cbxConfiguration->currentText());
        ui->cbxConfiguration->removeItem(ui->cbxConfiguration->currentIndex());
        this->on_btnNewConfiguration_clicked();
    }
}

void HydrodynamicDataDialog::on_btnNewConfiguration_clicked() {
    currentConfiguration = unsavedConfiguration;
    currentGridDataConfiguration = nullptr;
    ui->cbxConfiguration->setCurrentIndex(-1);
    ui->cbxGridDataConfiguration->setCurrentIndex(-1);
    ui->edtConfigurationName->setText("");
    ui->edtConfigurationName->setFocus();
    ui->tblBoundaryConditions->setRowCount(0);
    ui->btnEditBoundaryCondition->setEnabled(false);
    ui->btnRemoveBoundaryCondition->setEnabled(false);
    ui->vtkWidget->getMouseInteractor()->setHydrodynamicConfiguration(nullptr);
}

void HydrodynamicDataDialog::on_btnApplyConfiguration_clicked() {
    QString oldConfigurationName = ui->cbxConfiguration->currentText();
    QString newConfigurationName = ui->edtConfigurationName->text();
    
    if (newConfigurationName.isEmpty()) {
        QMessageBox::warning(this, tr("Hydrodynamic Data"), tr("Configuration name can't be empty."));
        return;
    }
    
    if (ui->cbxGridDataConfiguration->currentIndex() == -1) {
        QMessageBox::warning(this, tr("Hydrodynamic Data"), tr("Grid data configuration can't be empty."));
        return;
    }
    
    for (HydrodynamicParameter *parameter : currentConfiguration->getParameters()) {
        if (parameter->isProcessInput()) {
            QTreeWidgetItem *item = parameter->getItemWidget();
            QLineEdit *lineEdit = static_cast<QLineEdit*>(ui->trwParameters->itemWidget(item, 1));
            
            parameter->setSelected(!item->isHidden());
            
            if (parameter->isSelected()) {
                if (parameter->getParentValue() > -1) {
                    parameter->getParent()->setValue(parameter->getParentValue());
                }
                
                if (lineEdit) {
                    double value = lineEdit->text().toDouble();
                    
                    if (parameter->isInRange(value)) {
                        parameter->setValue(value);
                    } else {
                        double rangeMinimum = parameter->getRangeMinimum();
                        double rangeMaximum = parameter->getRangeMaximum();
                        QString message = QString("%1 must be between %2 and %3.").arg(parameter->getLabel()).arg(rangeMinimum).arg(rangeMaximum);
                        
                        QMessageBox::warning(this, "Hydrodynamic Data", message);
                        return;
                    }
                }
            }
        } else if (parameter->isInitialCondition()) {
            QLineEdit *lineEdit = static_cast<QLineEdit*>(ui->initialConditionsTab->findChild<QLineEdit*>(parameter->getName()));
            parameter->setValue(lineEdit->text().toDouble());
        }
    }
    
    currentConfiguration->setName(newConfigurationName);

    ui->cbxConfiguration->blockSignals(true);
    if (oldConfigurationName.isEmpty()) { // new configuration
        IPHApplication::getCurrentProject()->addHydrodynamicConfiguration(currentConfiguration);
        unsavedConfiguration = new HydrodynamicConfiguration();

        ui->cbxConfiguration->addItem(newConfigurationName);
        ui->cbxConfiguration->setCurrentText(newConfigurationName);
    } else {
        ui->cbxConfiguration->setItemText(ui->cbxConfiguration->currentIndex(), newConfigurationName);
    }
    ui->cbxConfiguration->blockSignals(false);
}

void HydrodynamicDataDialog::onToggleLabelsClicked(bool checked) {
    for (BoundaryCondition *boundaryCondition : currentConfiguration->getBoundaryConditions()) {
        boundaryCondition->getLabelsActor()->SetVisibility(checked);
    }
    ui->vtkWidget->update();
}

void HydrodynamicDataDialog::on_trwProcesses_itemChanged(QTreeWidgetItem *item, int column) {
    QTreeWidgetItem *parent = item->parent();
    HydrodynamicProcess *process = hydrodynamicDataRepository->findProcessByName(item->data(0, Qt::UserRole).toString());
    
    process->setChecked(item->checkState(0) == Qt::Checked);
    
    if (parent) {
        if (process->isChecked()) {
            // Unchecks all item siblings
            QList<HydrodynamicProcess*> siblings = process->getSiblings();
            
            for (int i = 0; i < siblings.size(); i++) {
                siblings[i]->setChecked(false);
            }
            
            // Checks parent if it is a checkable group
            if (process->getParent()->isCheckableGroup() && !process->getParent()->isChecked()) {
                process->getParent()->setChecked(true);
            } else {
                // Checks the first parent's child if all children are unchecked
                if (process->isCheckableGroup() && !process->hasChildChecked()) {
                    process->getChildren().first()->setChecked(true);
                }
            }
        } else {
            if (process->isCheckableGroup()) {
                ui->trwProcesses->blockSignals(true);
                if (process->hasSiblingChecked()) { // Unchecks all children items
                    process->uncheckChildren();
                } else { // If all reserved items are unchecked check item
                    process->setChecked(true);
                }
                ui->trwProcesses->blockSignals(false);
            } else {
                if (process->getParent()->isCheckableGroup()) {
                    if (!process->isChecked() && !process->getParent()->hasChildChecked()) { // If parent has a checked child does nothing
                        ui->trwProcesses->blockSignals(true);
                        process->getParent()->setChecked(true);
                        process->setChecked(true);
                        ui->trwProcesses->blockSignals(false);
                    }
                } else {
                    if (!process->hasSiblingChecked()) {
                        ui->trwProcesses->blockSignals(true);
                        process->setChecked(true);
                        ui->trwProcesses->blockSignals(false);
                    }
                }
            }
        }
    }
    
    if (process->isCheckable()) {
        HydrodynamicParameter *parameter = process->getTargetParameter();
        parameter->toggleHierarchyVisibility(process->isChecked());
    }
}

void HydrodynamicDataDialog::on_btnAddBoundaryCondition_clicked() {
    boundaryConditionDialog = new BoundaryConditionDialog(currentConfiguration, nullptr);
    boundaryConditionDialog->setHydrodynamicDataDialog(this);
    boundaryConditionDialog->show();
    
    toggleWidgets(false);
}

void HydrodynamicDataDialog::on_btnEditBoundaryCondition_clicked() {
    int row = ui->tblBoundaryConditions->currentRow();
    BoundaryCondition *boundaryCondition = currentConfiguration->getBoundaryCondition(row);
    boundaryConditionDialog = new BoundaryConditionDialog(currentConfiguration, boundaryCondition);
    
    boundaryConditionDialog->setHydrodynamicDataDialog(this);
    boundaryConditionDialog->show();
    
    toggleWidgets(false);
}

void HydrodynamicDataDialog::on_btnRemoveBoundaryCondition_clicked() {
    int currentRow = ui->tblBoundaryConditions->currentRow();
    QString question = tr("Are you sure you want to remove the selected boundary condtion?");
    
    if (currentRow > -1 && QMessageBox::question(this, tr("Hydrodynamic Data"), question) == QMessageBox::Yes) {
        BoundaryCondition *boundaryCondition = currentConfiguration->getBoundaryCondition(currentRow);
        
        ui->vtkWidget->getMouseInteractor()->removeBoundaryCondition(boundaryCondition);
        ui->tblBoundaryConditions->removeRow(currentRow);
        currentConfiguration->removeBoundaryCondition(currentRow);
        
        if (ui->tblBoundaryConditions->rowCount() == 0) {
            ui->btnEditBoundaryCondition->setEnabled(false);
            ui->btnRemoveBoundaryCondition->setEnabled(false);
        }
    }
}

void HydrodynamicDataDialog::closeEvent(QCloseEvent *event) {
    if (boundaryConditionDialog && boundaryConditionDialog->isVisible()) {
        event->ignore();
    } else {
        boundaryConditionDialog = nullptr;
        QDialog::closeEvent(event);
    }
}

void HydrodynamicDataDialog::toggleWidgets(bool enable) {
    for (int i = 0; i < ui->leftLayout->count(); i++) {
        QWidget *widget = ui->leftLayout->itemAt(i)->widget();
        
        if (widget) {
            widget->setEnabled(enable);
        }
    }
    
    for (int i = 0; i < ui->configurationLayout->count(); i++) {
        QWidget *widget = ui->configurationLayout->itemAt(i)->widget();
        
        if (widget) {
            widget->setEnabled(enable);
        }
    }
    
    for (int i = 0; i < ui->actionsLayout->count(); i++) {
        QWidget *widget = ui->actionsLayout->itemAt(i)->widget();
        
        if (widget) {
            widget->setEnabled(enable);
        }
    }
}

void HydrodynamicDataDialog::on_tblBoundaryConditions_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    if (current && (!previous || current->row() != previous->row())) {
        BoundaryCondition *boundaryCondition = currentConfiguration->getBoundaryCondition(current->row());
        
        ui->vtkWidget->getMouseInteractor()->highlightBoundaryCondition(boundaryCondition, true);
        
        if (previous) {
            boundaryCondition = currentConfiguration->getBoundaryCondition(previous->row());
            if (boundaryCondition) { // If a item is removed this slot will be called
                ui->vtkWidget->getMouseInteractor()->highlightBoundaryCondition(boundaryCondition, false);
            }
        }
        
        ui->btnEditBoundaryCondition->setEnabled(true);
        ui->btnRemoveBoundaryCondition->setEnabled(true);
    }
}

void HydrodynamicDataDialog::clearLayout(QLayout *layout) {
    QLayoutItem *item;
    
    while((item = layout->takeAt(0))) {
        if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}


bool HydrodynamicDataDialog::isCellLabelsActionChecked() const {
    return this->toggleCellLabelsAction->isChecked();
}

void HydrodynamicDataDialog::toggleZoomAreaAction(bool enable) {
    this->zoomAreaAction->setChecked(false);
    this->zoomAreaAction->setEnabled(enable);
}