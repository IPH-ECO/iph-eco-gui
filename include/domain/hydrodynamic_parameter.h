#ifndef HYDRODYNAMIC_PARAMETER_H
#define HYDRODYNAMIC_PARAMETER_H

#include <QTreeWidgetItem>
#include <QString>
#include <QList>

class HydrodynamicParameter {
private:
    QString name;
    QString label;
    HydrodynamicParameter *parent;
    bool editable;
    bool hideSiblings;
    double value;
    double defaultValue;
    double rangeMininum;
    double rangeMaximum;
    QList<HydrodynamicParameter*> children;
    QTreeWidgetItem* itemWidget;
public:
    HydrodynamicParameter() : parent(nullptr), editable(true), hideSiblings(true), rangeMininum(0), rangeMaximum(0) {}
    
    QString getName() const {
        return name;
    }
    
    void setName(const QString& name) {
        this->name = name;
    }

    QString getLabel() const {
        return label;
    }
    
    void setLabel(const QString &label) {
        this->label = label;
    }
    
    HydrodynamicParameter* getParent() const {
        return parent;
    }
    
    void setParent(HydrodynamicParameter *parent) {
        this->parent = parent;
        
        if (parent != nullptr) {
            parent->children.append(this);
        }
    }
    
    double getValue() const {
        return value;
    }
    
    void setValue(double value) {
        this->value = value;
    }
    
    bool isEditable() const {
        return editable;
    }
    
    void setEditable(bool editable) {
        this->editable = editable;
    }
    
    bool isSiblingsHidden() const {
        return hideSiblings;
    }
    
    void setSiblingsHidden(bool hideSiblings) {
        this->hideSiblings = hideSiblings;
    }

    double getDefaultValue() const {
        return defaultValue;
    }

    void setDefaultValue(double defaultValue) {
        this->defaultValue = defaultValue;
    }

    double getRangeMinimum() const {
        return rangeMininum;
    }

    void setRangeMinimum(double rangeMininum) {
        this->rangeMininum = rangeMininum;
    }

    double getRangeMaximum() const {
        return rangeMaximum;
    }

    void setRangeMaximum(double rangeMaximum) {
        this->rangeMaximum = rangeMaximum;
    }
    
    inline bool isInRange(double value) const {
        return (rangeMininum == 0 && rangeMaximum == 0) || (value >= rangeMininum && value <= rangeMaximum);
    }
    
    void toggleHierarchyVisibility(bool hide) const {
        if (hideSiblings) {
            QList<HydrodynamicParameter*> siblings = getSiblings();
            
            for (int i = 0; i < siblings.size(); i++) {
                siblings[i]->getItemWidget()->setHidden(hide);
            }
        }
        
        this->itemWidget->setHidden(!hide);
    }

    QList<HydrodynamicParameter*> getSiblings() const {
        QList<HydrodynamicParameter*> siblings;
        
        if (parent != nullptr) {
            for (int i = 0; i < parent->children.size(); i++) {
                HydrodynamicParameter *sibling = parent->children[i];
                
                if (sibling != this) {
                    siblings.append(sibling);
                }
            }
        }
        
        return siblings;
    }
    
    QTreeWidgetItem* getItemWidget() const {
        return itemWidget;
    }
    
    void setItemWidget(QTreeWidgetItem *itemWidget) {
        this->itemWidget = itemWidget;
    }
};

#endif // HYDRODYNAMIC_PARAMETER_H
