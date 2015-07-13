#ifndef HYDRODYNAMIC_DATA_NODE_H
#define HYDRODYNAMIC_DATA_NODE_H

#include <QString>
#include <QTreeWidgetItem>

class HydrodynamicDataNode {
private:
    QString parameterName;
    QString parameterParentName;
    QTreeWidgetItem *item;
    bool exclusive;
    bool visible;
    //QString jsonValue;
    
    // Transient
    bool siblingsVisible;

public:
    HydrodynamicDataNode();
    HydrodynamicDataNode(QTreeWidgetItem *item, bool siblingsVisible);
    
    QString getParameterName() const;
    QString getParameterParentName() const;
    bool isExclusive() const;
    void setExclusive(bool exclusive);
    bool isVisible() const;
    void setVisible(bool visible);
    QTreeWidgetItem* getItem() const;
    void setItem(QTreeWidgetItem *item);
    bool isSiblingsVisible() const;
    void setSiblingsVisible(bool siblingsVisible);
};

Q_DECLARE_METATYPE(HydrodynamicDataNode)

#endif // HYDRODYNAMIC_DATA_NODE_H
