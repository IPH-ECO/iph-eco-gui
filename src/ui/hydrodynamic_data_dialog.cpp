#include "include/ui/hydrodynamic_data_dialog.h"
#include "ui_hydrodynamic_data_dialog.h"
#include "include/domain/hydrodynamic_data_node.h"

#include <QTreeWidgetItemIterator>
#include <QRadioButton>
#include <QLineEdit>
#include <QMap>
#include <iostream>

Q_DECLARE_METATYPE(QTreeWidgetItem*)

HydrodynamicDataDialog::HydrodynamicDataDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::HydrodynamicDataDialog)
{
    ui->setupUi(this);
    ui->trwParameters->header()->setStretchLastSection(false);
    ui->trwParameters->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    this->hideParametersItems();
    
    QTreeWidgetItemIterator it(ui->trwProcesses, QTreeWidgetItemIterator::All);
    
    ui->trwProcesses->blockSignals(true);
    while (*it) {
        QTreeWidgetItem *item = *it;

        if (isCheckable(item)) {
            item->setCheckState(0, Qt::Unchecked);
        }
        item->setExpanded(true);
        
        it++;
    }
    ui->trwProcesses->blockSignals(false);
    
    it = QTreeWidgetItemIterator(ui->trwParameters, QTreeWidgetItemIterator::All);
    
    while (*it) {
        (*it)->setExpanded(true);
        it++;
    }

    this->buildParametersMap();
//    this->setupDefaultItems();
}

HydrodynamicDataDialog::~HydrodynamicDataDialog() {
    delete ui;
}

void HydrodynamicDataDialog::buildParametersMap() {
    QStringList leafSiblingsParameters = {
        "Constant", "Linear function", "Smagorinsky Model", "Zero-Equation Model", "Generic Length Scale Model", "Mellor and Yamada Turbulence Model",
        "ELCOM Mixing Model", "Eulerian-Lagrangian Method", "Second Order MUSCL Method", "High-order ENO/WENO Method", "No advection scheme",
        "Barotropic and the baroclinic contributions to the hydrostatic pressure", "Non-hydrostatic pressure"
    };
    QStringList siblingsParameters = {
        "Turbulence Model in the bottom boundary layers", "Based on air density (kg/m3)"
    };
    
    ui->trwProcesses->blockSignals(true);
    for (QTreeWidgetItemIterator it(ui->trwProcesses, QTreeWidgetItemIterator::All); *it; it++) {
        QTreeWidgetItem *processItem = *it;
        QString processItemName = processItem->text(0);
        
        if (isCheckable(processItem)) {
            if (siblingsParameters.contains(processItemName, Qt::CaseInsensitive)) {
                QTreeWidgetItem *parameterItem = ui->trwParameters->findItems(processItemName, Qt::MatchStartsWith | Qt::MatchRecursive).first();
                processItem->setData(0, Qt::UserRole, QVariant::fromValue(parameterItem));
                continue;
            }
            
            if (leafSiblingsParameters.contains(processItemName, Qt::CaseInsensitive) && !isCheckableProcess(processItem->parent())) {
                QString processParentItemText = processItem->parent()->text(0);
                QTreeWidgetItem *parentParameterItem = ui->trwParameters->findItems(processParentItemText, Qt::MatchStartsWith | Qt::MatchRecursive).first();
                
                for (int i = 0; i < parentParameterItem->childCount(); i++) {
                    QTreeWidgetItem *child = parentParameterItem->child(i);
                    
                    if (child->text(0) == processItemName) {
                        processItem->setData(0, Qt::UserRole, QVariant::fromValue(child));
                        break;
                    }
                }
                continue;
            }
        }
    }
    ui->trwProcesses->blockSignals(false);
}

void HydrodynamicDataDialog::hideParametersItems() {
    QStringList hiddenItems {
        "Wind Stress on Water Surface", "Bottom Roughness", "Viscosity/Diffusivity (Turbulence Model)",
        "Advection Scheme for Momentum Equation", "Pressure", "Coriolis factor"
    };
    
    for (int i = 0; i < hiddenItems.size(); i++) {
        QTreeWidgetItem *item = ui->trwParameters->findItems(hiddenItems[i], Qt::MatchExactly | Qt::MatchRecursive).first();
        
        for (int j = 0; j < item->childCount(); j++) {
            item->child(j)->setHidden(true);
        }
    }
}

bool HydrodynamicDataDialog::isCheckable(QTreeWidgetItem *item) const {
    return item->childCount() == 0 || isCheckableProcess(item);
}

bool HydrodynamicDataDialog::isCheckableProcess(QTreeWidgetItem *item) const {
    return QStringList({ "Chezy", "Manning", "White-Colebrook" }).contains(item->text(0).split(" ")[0]);
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

void HydrodynamicDataDialog::on_trwProcesses_itemChanged(QTreeWidgetItem *item, int column) {
    QTreeWidgetItem *parent = item->parent();
    
    if (parent == nullptr) {
        return;
    }
    
    if (item->checkState(0) == Qt::Checked) {
        // Unchecks all item siblings
        for (int i = 0; i < parent->childCount(); i++) {
            QTreeWidgetItem *child = parent->child(i);
            
            if (child != item && isCheckable(child)) { // Items with checkboxes
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
                if (!hasChildChecked(parent, item)) {
                    ui->trwProcesses->blockSignals(true);
                    item->setCheckState(0, Qt::Checked);
                    ui->trwProcesses->blockSignals(false);
                }
            }
        }
    }
    
    if (isCheckable(item)) {
        QVariant qVariant = item->data(0, Qt::UserRole);
        
        if (qVariant.isValid()) {
            QTreeWidgetItem *parameterItem = item->data(0, Qt::UserRole).value<QTreeWidgetItem*>();
            
            if (parameterItem != nullptr) {
                QTreeWidgetItem *parameterParentItem = parameterItem->parent();
                
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
                
                parameterItem->setHidden(item->checkState(0) == Qt::Unchecked);
            }
        }
    }
}

void HydrodynamicDataDialog::setupDefaultItems() {
    QTreeWidgetItem *windDrag = ui->trwProcesses->findItems("Based on Wind Drag coefficient", Qt::MatchExactly | Qt::MatchRecursive).first();
    
    windDrag->child(0)->setCheckState(0, Qt::Checked);
}
