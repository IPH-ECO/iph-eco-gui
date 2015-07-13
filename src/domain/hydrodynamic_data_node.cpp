#include "include/domain/hydrodynamic_data_node.h"
#include <QObject>

HydrodynamicDataNode::HydrodynamicDataNode() {}

HydrodynamicDataNode::HydrodynamicDataNode(QTreeWidgetItem *item, bool siblingsVisible) : item(item), siblingsVisible(siblingsVisible) {}

QString HydrodynamicDataNode::getParameterName() const {
    return parameterName;
}

QString HydrodynamicDataNode::getParameterParentName() const {
    return parameterParentName;
}

bool HydrodynamicDataNode::isExclusive() const {
    return exclusive;
}

void HydrodynamicDataNode::setExclusive(bool exclusive) {
    this->exclusive = exclusive;
}

bool HydrodynamicDataNode::isVisible() const {
    return visible;
}

void HydrodynamicDataNode::setVisible(bool visible) {
    this->visible = visible;
}

QTreeWidgetItem* HydrodynamicDataNode::getItem() const {
    return item;
}

void HydrodynamicDataNode::setItem(QTreeWidgetItem *item) {
    this->item = item;
}

bool HydrodynamicDataNode::isSiblingsVisible() const {
    return siblingsVisible;
}

void HydrodynamicDataNode::setSiblingsVisible(bool siblingsVisible) {
    this->siblingsVisible = siblingsVisible;
}