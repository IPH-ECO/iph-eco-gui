#include <domain/hydrodynamic_parameter.h>

HydrodynamicParameter::HydrodynamicParameter() :
    id(0),
    selected(false),
    parent(nullptr),
    editable(true),
    hideSiblings(true),
    rangeMininum(0),
    rangeMaximum(0),
    itemWidget(nullptr)
{}

uint HydrodynamicParameter::getId() const {
    return id;
}

void HydrodynamicParameter::setId(uint id) {
    this->id = id;
}

bool HydrodynamicParameter::isPersisted() const {
    return id != 0;
}

QString HydrodynamicParameter::getName() const {
    return name;
}

void HydrodynamicParameter:: setName(const QString& name) {
    this->name = name;
}

QString HydrodynamicParameter::getLabel() const {
    return label;
}

void HydrodynamicParameter::setLabel(const QString &label) {
    this->label = label;
}

double HydrodynamicParameter::getValue() const {
    return value;
}

void HydrodynamicParameter::setValue(double value) {
    this->value = value;
}

bool HydrodynamicParameter::isSelected() const {
    return selected;
}

void HydrodynamicParameter::setSelected(bool selected) {
    this->selected = selected;
}

void HydrodynamicParameter::setEnabled(bool enabled) {
    this->enabled = enabled;
}

bool HydrodynamicParameter::isEnabled() const {
    return enabled;
}

HydrodynamicParameterType HydrodynamicParameter::getType() const {
    return type;
}

void HydrodynamicParameter::setType(const HydrodynamicParameterType &type) {
    this->type = type;
}

HydrodynamicParameter* HydrodynamicParameter::getParent() const {
    return parent;
}

void HydrodynamicParameter::setParent(HydrodynamicParameter *parent) {
    this->parent = parent;
    
    if (parent) {
        parent->children.append(this);
    }
}

bool HydrodynamicParameter::isEditable() const {
    return editable;
}

void HydrodynamicParameter::setEditable(bool editable) {
    this->editable = editable;
}

bool HydrodynamicParameter::isSiblingsHidden() const {
    return hideSiblings;
}

void HydrodynamicParameter::setSiblingsHidden(bool hideSiblings) {
    this->hideSiblings = hideSiblings;
}

int HydrodynamicParameter::getParentValue() const {
    return parentValue;
}

void HydrodynamicParameter::setParentValue(int parentValue) {
    this->parentValue = parentValue;
}

int HydrodynamicParameter::getOrder() const {
    return order;
}

void HydrodynamicParameter::setOrder(int order) {
    this->order = order;
}

double HydrodynamicParameter::getRangeMinimum() const {
    return rangeMininum;
}

void HydrodynamicParameter::setRangeMinimum(double rangeMininum) {
    this->rangeMininum = rangeMininum;
}

double HydrodynamicParameter::getRangeMaximum() const {
    return rangeMaximum;
}

void HydrodynamicParameter::setRangeMaximum(double rangeMaximum) {
    this->rangeMaximum = rangeMaximum;
}

QTreeWidgetItem* HydrodynamicParameter::getItemWidget() const {
    return itemWidget;
}

void HydrodynamicParameter::setItemWidget(QTreeWidgetItem *itemWidget) {
    this->itemWidget = itemWidget;
}

void HydrodynamicParameter::toggleHierarchyVisibility(bool hide) {
    if (hideSiblings) {
        for (HydrodynamicParameter *sibling : getSiblings()) {
            sibling->toggleSubTreeVisibility(hide);
        }
    }
    
    this->toggleSubTreeVisibility(!hide);
}

QList<HydrodynamicParameter*> HydrodynamicParameter::getSiblings() const {
    QList<HydrodynamicParameter*> siblings;
    
    if (parent) {
        for (HydrodynamicParameter *sibling : parent->children) {
            if (sibling != this) {
                siblings.append(sibling);
            }
        }
    }
    
    return siblings;
}

void HydrodynamicParameter::toggleSubTreeVisibility(bool hide) {
    this->itemWidget->setHidden(hide);
    
    for (HydrodynamicParameter *child : parent->children) {
        child->toggleSubTreeVisibility(hide);
    }
}

QList<HydrodynamicParameter*> HydrodynamicParameter::getChildren() const {
    return children;
}

HydrodynamicParameter* HydrodynamicParameter::getChild(int i) const {
    return children[i];
}

SimulationDataType::HydrodynamicParameter HydrodynamicParameter::toSimulationDataType() const {
    SimulationDataType::HydrodynamicParameter parameter;
    QByteArray name = this->name.toLocal8Bit();
    
    parameter.length = this->name.size();
    parameter.name = new char[parameter.length];
    strncpy(parameter.name, name.constData(), this->name.size());
    parameter.value = this->value;
    
    return parameter;
}

HydrodynamicParameterType HydrodynamicParameter::mapTypeFromString(const QString &typeStr) {
    if (typeStr == "initialCondition") {
        return HydrodynamicParameterType::INITIAL_CONDITION;
    }
    
    return HydrodynamicParameterType::PROCESS_INPUT;
}

QString HydrodynamicParameter::mapStringFromType(const HydrodynamicParameterType &type) {
    if (type == HydrodynamicParameterType::INITIAL_CONDITION) {
        return "initialCondition";
    }
    
    return "processInput";
}

bool HydrodynamicParameter::sort(HydrodynamicParameter *p1, HydrodynamicParameter *p2) {
    return p1->order < p2->order;
}