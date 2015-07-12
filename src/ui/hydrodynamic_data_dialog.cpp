#include "include/ui/hydrodynamic_data_dialog.h"
#include "ui_hydrodynamic_data_dialog.h"
#include "include/domain/hydrodynamic_data_node.h"

#include <QTreeWidgetItemIterator>
#include <QRadioButton>
#include <QLineEdit>
#include <QMap>

HydrodynamicDataDialog::HydrodynamicDataDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::HydrodynamicDataDialog)
{
    ui->setupUi(this);
    ui->trwParameters->header()->setStretchLastSection(false);
    ui->trwParameters->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    this->hideParametersItems();
    
    QTreeWidgetItemIterator it(ui->trwProcesses, QTreeWidgetItemIterator::HasChildren);
    
    while (*it) {
        (*it)->setExpanded(true);
        it++;
    }
    
    it = QTreeWidgetItemIterator(ui->trwParameters, QTreeWidgetItemIterator::HasChildren);
    
    while (*it) {
        (*it)->setExpanded(true);
        it++;
    }
    
    it = QTreeWidgetItemIterator(ui->trwProcesses, QTreeWidgetItemIterator::All);
    
    ui->trwProcesses->blockSignals(true);
    while (*it) {
        if ((*it)->childCount() == 0 || isCheckableProcess(*it)) {
            (*it)->setCheckState(0, Qt::Unchecked);
        }
        
        it++;
    }
    ui->trwProcesses->blockSignals(false);
    

//    it = QTreeWidgetItemIterator(ui->trwParameters, QTreeWidgetItemIterator::All);
//    
//    ui->trwProcesses->blockSignals(true);
//    while (*it) {
//        if (isLeafOnProcessTab(*it)) {
//            (*it)->setHidden(true);
//        }
//        
//        it++;
//    }
//    this->setupDefaultItems();
}

HydrodynamicDataDialog::~HydrodynamicDataDialog() {
    delete ui;
}

void HydrodynamicDataDialog::lookupParameters() {
    
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
            
            if (child != item && (isCheckableProcess(child) || child->childCount() == 0)) { // Items with checkboxes
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
                if (!hasChildChecked(parent, item)) {
                    ui->trwProcesses->blockSignals(true);
                    item->setCheckState(0, Qt::Checked);
                    ui->trwProcesses->blockSignals(false);
                }
            }
        }
    }
    
//    if (isLeafOnProcessTab(item)) {
//        toggleParameterItem(item);
//    }
}

void HydrodynamicDataDialog::setupDefaultItems() {
    QTreeWidgetItem *windDrag = ui->trwProcesses->findItems("Based on Wind Drag coefficient", Qt::MatchExactly | Qt::MatchRecursive).first();
    
    windDrag->child(0)->setCheckState(0, Qt::Checked);
}

void HydrodynamicDataDialog::setupParametersTree() {
    QMap<QString, QString> parameters {
        {"gravityAcceleration", "Gravity acceleration"},
        {"waterDensity", "Water density of reference"},
        {"waterTemperature", "Water temperature of reference"},
        {"airTemperature", "Air temperature of reference"},
        {"thetaCoefficient", "Theta coefficient"},
        {"thresholdDepthForWetAndDryAlgorithm", "Threshold depth for Wet and Dry Algorithm"},
        {"smoothingTime", "Smoothing time"}
    };

    for (QMap<QString, QString>::const_iterator it = parameters.constBegin(); it != parameters.constEnd(); it++) {
        QTreeWidgetItem *treeItem = ui->trwParameters->findItems(it.value(), Qt::MatchStartsWith | Qt::MatchRecursive).first();
        QLineEdit *lineEdit = new QLineEdit(ui->trwParameters);
        
        lineEdit->setObjectName(it.key());
        ui->trwParameters->setItemWidget(treeItem, 1, lineEdit);
    }
}

void HydrodynamicDataDialog::toggleParameterItem(QTreeWidgetItem *item) {
    QList<QTreeWidgetItem*> items = ui->trwParameters->findItems(item->text(0), Qt::MatchExactly | Qt::MatchRecursive);
    QTreeWidgetItem *processParent = item->parent();
    QTreeWidgetItem *parameterItem = nullptr;
    
    // Find correspondent item on parameter tab
    for (int i = 0; i < items.size(); i++) {
        if (processParent->text(0) == items[i]->parent()->text(0)) {
            parameterItem = items[i];
            break;
        }
    }
    
    QTreeWidgetItem *parameterParent = parameterItem->parent();
    
    for (int i = 0; i < parameterParent->childCount(); i++) {
        QTreeWidgetItem *child = parameterParent->child(i);
        
        if (child != parameterItem) {
            child->setHidden(item->checkState(0) == Qt::Checked);
            
            for (int j = 0; j < child->childCount(); j++) {
                child->child(j)->setHidden(item->checkState(0) == Qt::Checked);
            }
        }
    }
}