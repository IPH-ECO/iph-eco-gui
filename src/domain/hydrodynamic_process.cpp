#include <domain/hydrodynamic_process.h>

HydrodynamicProcess::HydrodynamicProcess() : parent(nullptr), targetParameter(nullptr), checkable(false), itemWidget(nullptr) {}
    
QString HydrodynamicProcess::getName() const {
    return name;
}

void HydrodynamicProcess::setName(const QString &name) {
    this->name = name;
}

QString HydrodynamicProcess::getLabel() const {
    return label;
}

void HydrodynamicProcess::setLabel(const QString &label) {
    this->label = label;
}

bool HydrodynamicProcess::isCheckable() const {
    return checkable;
}

void HydrodynamicProcess::setCheckable(bool checkable) {
    this->checkable = checkable;
}

bool HydrodynamicProcess::isChecked() const {
    return checked;
}

void HydrodynamicProcess::setChecked(bool checked) {
    this->checked = checked;

    if (this->checkable && this->itemWidget) {
        Qt::CheckState checkState = this->itemWidget->checkState(0);
        bool changeCheckState = (!checked && checkState == Qt::Checked) || (checked && checkState == Qt::Unchecked);
        
        if (changeCheckState) {
            this->itemWidget->setCheckState(0, checked ? Qt::Checked : Qt::Unchecked);
        }
    }
}

HydrodynamicProcess* HydrodynamicProcess::getParent() const {
    return parent;
}

void HydrodynamicProcess::setParent(HydrodynamicProcess *parent) {
    this->parent = parent;
    
    if (parent) {
        parent->children.append(this);
    }
}

HydrodynamicParameter* HydrodynamicProcess::getTargetParameter() const {
    return targetParameter;
}

void HydrodynamicProcess::setTargetParameter(HydrodynamicParameter *targetParameter) {
    this->targetParameter = targetParameter;
    this->itemWidget = nullptr;
}

bool HydrodynamicProcess::isCheckableGroup() const {
    return checkable == true && children.size() > 0;
}

QTreeWidgetItem* HydrodynamicProcess::getItemWidget() const {
    return itemWidget;
}

void HydrodynamicProcess::setItemWidget(QTreeWidgetItem *itemWidget) {
    this->itemWidget = itemWidget;
    
    if (this->checkable) {
        this->itemWidget->setCheckState(0, checked ? Qt::Checked : Qt::Unchecked);
    }
}

QList<HydrodynamicProcess*> HydrodynamicProcess::getChildren() const {
    return children;
}

QList<HydrodynamicProcess*> HydrodynamicProcess::getSiblings() const {
    QList<HydrodynamicProcess*> siblings;
    
    if (parent) {
        for (int i = 0; i < parent->children.size(); i++) {
            HydrodynamicProcess *sibling = parent->children[i];
            
            if (sibling != this) {
                siblings.append(sibling);
            }
        }
    }

    return siblings;
}

bool HydrodynamicProcess::hasChildChecked() const {
    for (int i = 0; i < children.size(); i++) {
        if (children[i]->isCheckable() && children[i]->isChecked()) {
            return true;
        }
    }
    
    return false;
}

bool HydrodynamicProcess::hasSiblingChecked() const {
    QList<HydrodynamicProcess*> siblings = getSiblings();
    
    for (int i = 0; i < siblings.size(); i++) {
        HydrodynamicProcess *sibling = siblings[i];
        
        if (!sibling->isCheckable() || sibling->isChecked()) {
            return true;
        }
    }
    
    return false;
}

void HydrodynamicProcess::uncheckChildren() {
    for (int i = 0; i < children.size(); i++) {
        HydrodynamicProcess *child = children[i];
        if (child->isCheckable() && child->isChecked()) {
            child->setChecked(false);
            child->getItemWidget()->setCheckState(0, Qt::Unchecked);
        }
    }
}

void HydrodynamicProcess::uncheckSiblings() {
    QList<HydrodynamicProcess*> siblings = getSiblings();
    
    for (int i = 0; i < siblings.size(); i++) {
        HydrodynamicProcess *sibling = siblings[i];
        
        if (sibling->isCheckable() && sibling->isChecked()) {
            sibling->setChecked(false);
            sibling->getItemWidget()->setCheckState(0, Qt::Unchecked);
        }
    }
}