#include "include/ui/hydrodynamic_data_dialog.h"
#include "ui_hydrodynamic_data_dialog.h"

#include "include/application/iph_application.h"
#include "include/domain/hydrodynamic_parameter.h"

#include <QTreeWidgetItemIterator>
#include <QMessageBox>
#include <QLineEdit>
#include <QSet>

HydrodynamicDataDialog::HydrodynamicDataDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::HydrodynamicDataDialog), unsavedConfiguration(new HydrodynamicConfiguration), currentConfiguration(unsavedConfiguration)
{
    hydrodynamicDataRepository = HydrodynamicDataRepository::getInstance();
    
    ui->setupUi(this);
    ui->trwParameters->header()->setStretchLastSection(false);
    ui->trwParameters->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->btnRemove->setEnabled(false);
    
    Project *project = IPHApplication::getCurrentProject();
    QSet<HydrodynamicConfiguration*> configurations = project->getHydrodynamicConfigurations();
    
    ui->cbxConfiguration->blockSignals(true);
    for (QSet<HydrodynamicConfiguration*>::const_iterator it = configurations.begin(); it != configurations.end(); ++it) {
        ui->cbxConfiguration->addItem((*it)->getName());
    }
    ui->cbxConfiguration->setCurrentIndex(-1);
    ui->cbxConfiguration->blockSignals(false);

    this->setupItems();
}

HydrodynamicDataDialog::~HydrodynamicDataDialog() {
    delete unsavedConfiguration;
    delete ui;
}

void HydrodynamicDataDialog::addParameterItemWidget(HydrodynamicParameter *parameter) {
    if (parameter->isEnvironmentVariable() || parameter->getItemWidget()) {
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
        ui->trwParameters->setItemWidget(item, 1, lineEdit);
    }
    
    for (int i = 0; i < parameter->getChildren().size(); i++) {
        addParameterItemWidget(parameter->getChild(i));
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
        
        if (process->getParent() == nullptr) {
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
    
    this->expandTrees();
}

void HydrodynamicDataDialog::expandTrees() {
    QTreeWidgetItemIterator it(ui->trwProcesses, QTreeWidgetItemIterator::All);
    
    while (*it) {
        (*it)->setExpanded(true);
        it++;
    }
    
    it = QTreeWidgetItemIterator(ui->trwParameters, QTreeWidgetItemIterator::All);
    
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
    bool isConfigurationNamePresent = !configurationName.isEmpty();
    
    currentConfiguration->removeItemWidgets();

    if (isConfigurationNamePresent) {
        Project *project = IPHApplication::getCurrentProject();
        currentConfiguration = project->getHydrodynamicConfiguration(configurationName);
        
        ui->edtConfigurationName->setText(currentConfiguration->getName());
    } else {
        ui->edtConfigurationName->clear();
        currentConfiguration = unsavedConfiguration;
    }
    
    ui->btnDone->setEnabled(isConfigurationNamePresent);
    ui->btnRemove->setEnabled(isConfigurationNamePresent);
    ui->trwProcesses->clear();
    ui->trwParameters->clear();
    
    this->setupItems();
}

void HydrodynamicDataDialog::on_btnRemove_clicked() {
    QMessageBox::StandardButton question = QMessageBox::question(this, tr("Hydrodynamic Data"), tr("Are you sure you want to remove the selected configuration?"));
    
    if (question == QMessageBox::Yes) {
        IPHApplication::getCurrentProject()->removeHydrodynamicConfiguration(ui->cbxConfiguration->currentText());
        currentConfiguration = unsavedConfiguration;
        
        ui->cbxConfiguration->removeItem(ui->cbxConfiguration->currentIndex());
        ui->cbxConfiguration->setCurrentIndex(-1);
    }
}

void HydrodynamicDataDialog::on_btnDone_clicked() {
    ui->cbxConfiguration->setCurrentIndex(-1);
}

void HydrodynamicDataDialog::on_btnSave_clicked() {
    QString configurationName = ui->edtConfigurationName->text();
    
    if (configurationName.isEmpty()) {
        QMessageBox::warning(this, tr("Hydrodynamic Data"), tr("Configuration name can't be empty."));
        return;
    }
    
    QList<HydrodynamicParameter*> parameters = currentConfiguration->getParameters();
    
    for (int i = 0; i < parameters.size(); i++) {
        HydrodynamicParameter *parameter = parameters[i];
        
        if (parameter->isProcessInput()) {
            QTreeWidgetItem *item = parameter->getItemWidget();
            QLineEdit *lineEdit = static_cast<QLineEdit*>(ui->trwParameters->itemWidget(item, 1));
            
            parameter->setSelected(!item->isHidden());
            
            if (parameter->isSelected()) {
                if (parameter->getParentValue() > -1) {
                    parameter->getParent()->setValue(parameter->getParentValue());
                }
                
                if (lineEdit != nullptr) {
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
        } else if (parameter->isEnvironmentVariable()) {
            
        }
    }
    
    currentConfiguration->setName(configurationName);

    ui->cbxConfiguration->blockSignals(true);
    if (ui->cbxConfiguration->currentIndex() == -1) {
        IPHApplication::getCurrentProject()->addHydrodynamicConfiguration(currentConfiguration);
        unsavedConfiguration = new HydrodynamicConfiguration();

        ui->cbxConfiguration->addItem(configurationName);
        ui->cbxConfiguration->setCurrentText(configurationName);
    } else {
        ui->cbxConfiguration->setItemText(ui->cbxConfiguration->currentIndex(), configurationName);
        ui->btnRemove->setEnabled(true);
        ui->btnDone->setEnabled(true);
    }
    ui->cbxConfiguration->blockSignals(false);
    
    ui->btnDone->setEnabled(true);
}

void HydrodynamicDataDialog::on_trwProcesses_itemChanged(QTreeWidgetItem *item, int column) {
    QTreeWidgetItem *parent = item->parent();
    HydrodynamicProcess *process = hydrodynamicDataRepository->findProcessByName(item->data(0, Qt::UserRole).toString());
    
    process->setChecked(item->checkState(0) == Qt::Checked);
    
    if (parent != nullptr) {
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