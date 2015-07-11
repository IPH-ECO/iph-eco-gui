#include "include/ui/hydrodynamic_data_dialog.h"
#include "ui_hydrodynamic_data_dialog.h"

#include <QTreeWidgetItemIterator>
#include <QRadioButton>
#include <iostream>

HydrodynamicDataDialog::HydrodynamicDataDialog(QWidget *parent) : QDialog(parent), ui(new Ui::HydrodynamicDataDialog) {
    ui->setupUi(this);
    
    QTreeWidgetItemIterator ut(ui->trwProcesses, QTreeWidgetItemIterator::HasChildren);
    
    while (*ut) {
        (*ut)->setExpanded(true);
        ut++;
    }
    
    QTreeWidgetItemIterator it(ui->trwProcesses, QTreeWidgetItemIterator::NoChildren);
    
    ui->trwProcesses->blockSignals(true);
    while (*it) {
        (*it)->setCheckState(0, Qt::Unchecked);
        it++;
    }
    
    chezyItem = ui->trwProcesses->findItems("Chezy", Qt::MatchStartsWith | Qt::MatchRecursive).first();
    manningItem = ui->trwProcesses->findItems("Manning", Qt::MatchStartsWith | Qt::MatchRecursive).first();
    whiteColebrookItem = ui->trwProcesses->findItems("White-Colebrook", Qt::MatchStartsWith | Qt::MatchRecursive).first();
    
    chezyItem->setCheckState(0, Qt::Unchecked);
    manningItem->setCheckState(0, Qt::Unchecked);
    whiteColebrookItem->setCheckState(0, Qt::Unchecked);
    ui->trwProcesses->blockSignals(false);
}

HydrodynamicDataDialog::~HydrodynamicDataDialog() {
    delete ui;
}

void HydrodynamicDataDialog::on_trwProcesses_itemChanged(QTreeWidgetItem *item, int column) {
    QTreeWidgetItem *parent = item->parent();
    
    if (parent == nullptr) {
        return;
    }
    
    bool isReservedItem = item == chezyItem || item == manningItem || item == whiteColebrookItem;
    bool isParentReservedItem = parent == chezyItem || parent == manningItem || parent == whiteColebrookItem;
    
    if (item->checkState(0) == Qt::Checked) {
        // Unchecks all item siblings
        for (int i = 0; i < parent->childCount(); i++) {
            QTreeWidgetItem *child = parent->child(i);
            
            if (child != item) {
                child->setCheckState(0, Qt::Unchecked);
            }
        }
        
        // Checks parent if it is a reserved item
        if (isParentReservedItem && parent->checkState(0) == Qt::Unchecked) {
            parent->setCheckState(0, Qt::Checked);
        } else {
            // Checks the first parent's child if all children are unchecked
            if (isReservedItem) {
                bool hasChildChecked = false;
                
                for (int i = 0; i < item->childCount(); i++) {
                    if (item->child(i)->checkState(0) == Qt::Checked) {
                        hasChildChecked = true;
                        break;
                    }
                }
                
                if (!hasChildChecked) {
                    item->child(0)->setCheckState(0, Qt::Checked);
                }
            }
        }
    } else {
        if (isReservedItem) {
            bool areReservedItemsUnchecked = chezyItem->checkState(0) == Qt::Unchecked && manningItem->checkState(0) == Qt::Unchecked && whiteColebrookItem->checkState(0) == Qt::Unchecked;

            ui->trwProcesses->blockSignals(true);
            if (areReservedItemsUnchecked) { // If all reserved items are unchecked check item
                item->setCheckState(0, Qt::Checked);
            } else { // Unchecks all children items
                for (int i = 0; i < item->childCount(); i++) {
                    item->child(i)->setCheckState(0, Qt::Unchecked);
                }
            }
            ui->trwProcesses->blockSignals(false);
        } else {
            if (isParentReservedItem) {
                bool hasChildChecked = false;
                
                for (int i = 0; i < parent->childCount(); i++) {
                    if (parent->child(i)->checkState(0) == Qt::Checked) {
                        hasChildChecked = true;
                        break;
                    }
                }
                
                if (!hasChildChecked) { // If parent has a check child does nothing
                    ui->trwProcesses->blockSignals(true);
                    parent->setCheckState(0, Qt::Checked);
                    item->setCheckState(0, Qt::Checked);
                    ui->trwProcesses->blockSignals(false);
                }
            }
        }
    }
}
