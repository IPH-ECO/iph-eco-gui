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
    bool hideSiblings;
    int parentValue;
    int order;
    double rangeMininum;
    double rangeMaximum;
    QList<HydrodynamicParameter*> children;
    QTreeWidgetItem* itemWidget;
public:
    HydrodynamicParameter();

    uint getId() const;
    void setId(uint id);
    bool isPersisted() const;
    QString getName() const;
    void setName(const QString& name);
    QString getLabel() const;
    void setLabel(const QString &label);
    double getValue() const;
    void setValue(double value);
    bool isSelected() const;
    void setSelected(bool selected);
    HydrodynamicParameter* getParent() const;
    void setParent(HydrodynamicParameter *parent);
    bool isEditable() const;
    void setEditable(bool editable);
    bool isSiblingsHidden() const;
    void setSiblingsHidden(bool hideSiblings);
    int getParentValue() const;
    void setParentValue(int parentValue);
    int getOrder() const;
    void setOrder(int order);
    double getRangeMinimum() const;
    void setRangeMinimum(double rangeMininum);
    double getRangeMaximum() const;
    void setRangeMaximum(double rangeMaximum);
    QTreeWidgetItem* getItemWidget() const;
    void setItemWidget(QTreeWidgetItem *itemWidget);
    void toggleHierarchyVisibility(bool hide);
    QList<HydrodynamicParameter*> getSiblings() const;
    void toggleSubTreeVisibility(bool hide);
    QList<HydrodynamicParameter*> getChildren() const;
    HydrodynamicParameter* getChild(int i) const;
    
    static bool sort(HydrodynamicParameter *p1, HydrodynamicParameter *p2);
    inline bool isInRange(double value) const {
        return (rangeMininum == 0 && rangeMaximum == 0) || (value >= rangeMininum && value <= rangeMaximum);
    }
};

#endif // HYDRODYNAMIC_PARAMETER_H
