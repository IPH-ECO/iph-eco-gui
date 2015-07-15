#ifndef HYDRODYNAMIC_PROCESS_H
#define HYDRODYNAMIC_PROCESS_H

#include "include/domain/hydrodynamic_parameter.h"
#include <QTreeWidgetItem>
#include <QString>
#include <QList>

class HydrodynamicProcess {
private:
    uint id;
    QString name;
    QString label;
    HydrodynamicProcess *parent;
    HydrodynamicParameter *targetParameter;
    bool checkable;
    bool checked;
    QList<HydrodynamicProcess*> children;
    QTreeWidgetItem *widgetItem;
public:
    HydrodynamicProcess(const QString &name, const QString &label, HydrodynamicProcess *parent = nullptr, bool checkable = false, HydrodynamicParameter *targetParameter = nullptr) :
        name(name), label(label), parent(parent), targetParameter(targetParameter), checkable(checkable), checked(false)
    {
        if (parent != nullptr) {
            parent->children.append(this);
        }
    }
    
    QString getName() const {
        return name;
    }
    
    void setName(const QString &name) {
        this->name = name;
    }

    QString getLabel() const {
        return label;
    }
    
    void setLabel(const QString &label) {
        this->label = label;
    }

    bool isCheckable() const {
        return checkable;
    }

    void setCheckable(bool checkable) {
        this->checkable = checkable;
    }

    bool isChecked() const {
        return checked;
    }

    void setChecked(bool checked) {
        this->checked = checked;

        if (this->checkable && this->widgetItem != nullptr) {
            Qt::CheckState checkState = this->widgetItem->checkState(0);
            bool changeCheckState = (!checked && checkState == Qt::Checked) || (checked && checkState == Qt::Unchecked);
            
            if (changeCheckState) {
                this->widgetItem->setCheckState(0, checked ? Qt::Checked : Qt::Unchecked);
            }
        }
    }

    HydrodynamicProcess* getParent() const {
        return parent;
    }
    
    HydrodynamicParameter* getTargetParameter() const {
        return targetParameter;
    }

    bool isCheckableGroup() const {
        return checkable == true && children.size() > 0;
    }
    
    QTreeWidgetItem* getWidgetItem() const {
        return widgetItem;
    }
    
    void setWidgetItem(QTreeWidgetItem *widgetItem) {
        this->widgetItem = widgetItem;
        
        if (this->checkable) {
            this->widgetItem->setCheckState(0, checked ? Qt::Checked : Qt::Unchecked);
        }
    }
    
    QList<HydrodynamicProcess*> getChildren() const {
        return children;
    }
    
    QList<HydrodynamicProcess*> getSiblings() const {
        QList<HydrodynamicProcess*> siblings;
        
        if (parent != nullptr) {
            for (int i = 0; i < parent->children.size(); i++) {
                HydrodynamicProcess *sibling = parent->children[i];
                
                if (sibling != this) {
                    siblings.append(sibling);
                }
            }
        }
    
        return siblings;
    }
    
    bool hasChildChecked() const {
        for (int i = 0; i < children.size(); i++) {
            if (children[i]->isCheckable() && children[i]->isChecked()) {
                return true;
            }
        }
        
        return false;
    }
    
    bool hasSiblingChecked() const {
        QList<HydrodynamicProcess*> siblings = getSiblings();
        
        for (int i = 0; i < siblings.size(); i++) {
            if (siblings[i]->isCheckable() && siblings[i]->isChecked()) {
                return true;
            }
        }
        
        return false;
    }
    
    void uncheckChildren() {
        for (int i = 0; i < children.size(); i++) {
            HydrodynamicProcess *child = children[i];
            if (child->isCheckable() && child->isChecked()) {
                child->setChecked(false);
                child->getWidgetItem()->setCheckState(0, Qt::Unchecked);
            }
        }
    }
    
    void uncheckSiblings() {
        QList<HydrodynamicProcess*> siblings = getSiblings();
        
        for (int i = 0; i < siblings.size(); i++) {
            HydrodynamicProcess *sibling = siblings[i];
            
            if (sibling->isCheckable() && sibling->isChecked()) {
                sibling->setChecked(false);
                sibling->getWidgetItem()->setCheckState(0, Qt::Unchecked);
            }
        }
    }
};

#endif // HYDRODYNAMIC_PROCESS_H
