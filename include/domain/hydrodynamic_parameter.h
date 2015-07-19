#ifndef HYDRODYNAMIC_PARAMETER_H
#define HYDRODYNAMIC_PARAMETER_H

#include <QTreeWidgetItem>
#include <QString>
#include <QList>

class HydrodynamicParameter {
private:
    uint id;
    QString name;
    QString label;
    double value;
    bool selected;
    
    // Trasient attributes
    HydrodynamicParameter *parent;
    bool editable;
    bool persistable;
    bool hideSiblings;
    double rangeMininum;
    double rangeMaximum;
    QList<HydrodynamicParameter*> children;
    QTreeWidgetItem* itemWidget;
public:
    HydrodynamicParameter() : selected(false), parent(nullptr), editable(true), persistable(false), hideSiblings(true), rangeMininum(0), rangeMaximum(0) {}

    uint getId() const {
        return id;
    }

    void setId(uint id) {
        this->id = id;
    }

    bool isPersisted() const {
        return id != 0;
    }
    
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
    
    double getValue() const {
        return value;
    }
    
    void setValue(double value) {
        this->value = value;
    }
    
    bool isSelected() const {
        return selected;
    }
    
    void setSelected(bool selected) {
        this->selected = selected;
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
    
    bool isEditable() const {
        return editable;
    }
    
    void setEditable(bool editable) {
        this->editable = editable;
    }
    
    bool isPersistable() const {
        return persistable;
    }
    
    void setPersistable(bool persistable) {
        this->persistable = persistable;
    }
    
    bool isSiblingsHidden() const {
        return hideSiblings;
    }
    
    void setSiblingsHidden(bool hideSiblings) {
        this->hideSiblings = hideSiblings;
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
    
    QTreeWidgetItem* getItemWidget() const {
        return itemWidget;
    }
    
    void setItemWidget(QTreeWidgetItem *itemWidget) {
        this->itemWidget = itemWidget;
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
};

#endif // HYDRODYNAMIC_PARAMETER_H
