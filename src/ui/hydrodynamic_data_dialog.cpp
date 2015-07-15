#include "include/ui/hydrodynamic_data_dialog.h"
#include "ui_hydrodynamic_data_dialog.h"

#include "include/domain/hydrodynamic_parameter.h"

#include <QTreeWidgetItemIterator>
#include <QRadioButton>
#include <QLineEdit>
#include <QMap>
#include <iostream>

HydrodynamicDataDialog::HydrodynamicDataDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::HydrodynamicDataDialog), seed(new HydrodynamicDataSeed)
{
    ui->setupUi(this);
    ui->trwParameters->header()->setStretchLastSection(false);
    ui->trwParameters->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    this->setupItems();
    this->expandTrees();
}

HydrodynamicDataDialog::~HydrodynamicDataDialog() {
    delete ui;
}

void HydrodynamicDataDialog::setupItems() {
    QList<HydrodynamicProcess*> processes = seed->getProcesses();
    QList<HydrodynamicParameter*> parameters = seed->getParameters();

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

        if (process->isCheckable()) {
            item->setCheckState(0, process->isChecked() ? Qt::Checked : Qt::Unchecked);
        }
        item->setData(0, Qt::UserRole, QVariant(process->getName()));
    }
    ui->trwProcesses->blockSignals(false);
    
    ui->trwParameters->blockSignals(true);
    for (int i = 0; i < parameters.size(); i++) {
        HydrodynamicParameter *parameter = parameters[i];
        QTreeWidgetItem *item = nullptr;
        
        if (parameter->getParent() == nullptr) {
            item = new QTreeWidgetItem(ui->trwParameters, QStringList(parameter->getLabel()));
        } else {
            QTreeWidgetItem *parentItem = nullptr;
            QTreeWidgetItemIterator it(ui->trwParameters);
            
            while (*it) {
                QString itemName = (*it)->data(0, Qt::UserRole).toString();
                if (itemName == parameter->getParent()->getName()) {
                    parentItem = *it;
                    break;
                }
                it++;
            }
            
            item = new QTreeWidgetItem(parentItem, QStringList(parameter->getLabel()));
        }
        item->setData(0, Qt::UserRole, QVariant(parameter->getName()));
    }
    ui->trwParameters->blockSignals(false);
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

void HydrodynamicDataDialog::on_btnDone_clicked() {
    QLineEdit *edt = ui->trwParameters->findChild<QLineEdit*>("gravityAcceleration");
    std::cout << edt->text().toStdString();
}

bool HydrodynamicDataDialog::isCheckable(QTreeWidgetItem *item) const {
    return item->childCount() == 0 || isCheckableProcess(item);
}

bool HydrodynamicDataDialog::isCheckableProcess(QTreeWidgetItem *item) const {
    QString itemText = item->text(0);
    return QStringList({ "Chezy", "Manning", "White-Colebrook" }).contains(itemText.split(" ")[0]) || "Based on Wind Drag coefficient" == itemText;
}

bool HydrodynamicDataDialog::hasChildChecked(QTreeWidgetItem *item, QTreeWidgetItem *excludedItem) const {
    for (int i = 0; i < item->childCount(); i++) {
        QTreeWidgetItem *child = item->child(i);
        
        if (child != excludedItem && child->checkState(0) == Qt::Checked) {
            return true;
        }
    }
    
    return false;
}

QString HydrodynamicDataDialog::getCheckableProcessIdentifier(QTreeWidgetItem *item) {
    QString itemText = item->text(0);
    
    if (itemText == "Based on Wind Drag coefficient") {
        return "Wind drag coefficient";
    }
    
    return itemText.split(" ")[0];
}

void HydrodynamicDataDialog::on_trwProcesses_itemChanged(QTreeWidgetItem *item, int column) {
    QTreeWidgetItem *parent = item->parent();
    HydrodynamicProcess *process = seed->findProcessByName(item->data(0, Qt::UserRole).toString());
    
    if (parent != nullptr) {
        if (item->checkState(0) == Qt::Checked) {
            // Unchecks all item siblings
            for (int i = 0; i < parent->childCount(); i++) {
                QTreeWidgetItem *child = parent->child(i);
                
                if (child != item && process->isCheckable()) { // Items with checkboxes
                    child->setCheckState(0, Qt::Unchecked);
                }
            }
            
            // Checks parent if it is a reserved item
            if (isCheckableProcess(item->parent()) && parent->checkState(0) == Qt::Unchecked) {
                parent->setCheckState(0, Qt::Checked);
            } else {
                // Checks the first parent's child if all children are unchecked
                if (isCheckableProcess(item) && !hasChildChecked(item, nullptr)) {
                    item->child(0)->setCheckState(0, Qt::Checked);
                }
            }
        } else {
            if (isCheckableProcess(item)) {
                ui->trwProcesses->blockSignals(true);
                if (hasChildChecked(parent, item)) {  // Unchecks all children items
                    for (int i = 0; i < item->childCount(); i++) {
                        item->child(i)->setCheckState(0, Qt::Unchecked);
                    }
                } else { // If all reserved items are unchecked check item
                    item->setCheckState(0, Qt::Checked);
                }
                ui->trwProcesses->blockSignals(false);
            } else {
                if (isCheckableProcess(parent)) {
                    if (!hasChildChecked(parent, nullptr)) { // If parent has a checked child does nothing
                        ui->trwProcesses->blockSignals(true);
                        parent->setCheckState(0, Qt::Checked);
                        item->setCheckState(0, Qt::Checked);
                        ui->trwProcesses->blockSignals(false);
                    }
                } else {
                    // TODO: Fix implementation. A single item must be uncheckable
//                    if (!hasChildChecked(parent, item)) {
//                        ui->trwProcesses->blockSignals(true);
//                        item->setCheckState(0, Qt::Checked);
//                        ui->trwProcesses->blockSignals(false);
//                    }
                }
            }
        }
    }
    
    if (isCheckable(item)) {
        QVariant qVariant = item->data(0, Qt::UserRole);
        
        if (qVariant.isValid()) {
            HydrodynamicDataNode dataNode = item->data(0, Qt::UserRole).value<HydrodynamicDataNode>();
            QTreeWidgetItem *parameterItem = dataNode.getItem();
            
            if (parameterItem != nullptr) {
                QTreeWidgetItem *parameterParentItem = parameterItem->parent();
                
                if (!dataNode.isSiblingsVisible() && parameterParentItem != nullptr) {
                    for (int i = 0; i < parameterParentItem->childCount(); i++) {
                        QTreeWidgetItem *child = parameterParentItem->child(i);
                        
                        if (child != parameterParentItem) {
                            child->setHidden(item->checkState(0) == Qt::Checked);
                        }
                    }
                    
                    while (parameterParentItem != nullptr) {
                        parameterParentItem->setHidden(item->checkState(0) == Qt::Unchecked);
                        parameterParentItem = parameterParentItem->parent();
                    }
                }
                
                parameterItem->setHidden(item->checkState(0) == Qt::Unchecked);
                
                for (int i = 0; i < parameterItem->childCount(); i++) {
                    parameterItem->child(i)->setHidden(item->checkState(0) == Qt::Unchecked);
                }
            }
        }
    }
}