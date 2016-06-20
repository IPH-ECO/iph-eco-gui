#include <domain/water_quality_parameter.h>

WaterQualityParameter::WaterQualityParameter() :
	id(0),
    checked(false),
    enabled(true),
    parent(nullptr),
    editable(true),
    rangeMininum(0),
    rangeMaximum(0),
    itemWidget(nullptr)
{}

uint WaterQualityParameter::getId() const {
    return id;
}

void WaterQualityParameter::setId(const uint &id) {
    this->id = id;
}

bool WaterQualityParameter::isPersisted() const {
    return id != 0;
}

QString WaterQualityParameter::getName() const {
    return name;
}

void WaterQualityParameter::setName(const QString& name) {
    this->name = name;
}

double WaterQualityParameter::getValue() const {
    return value;
}

void WaterQualityParameter::setValue(double value) {
    this->value = value;
}

QList<double> WaterQualityParameter::getGroupValues() const {
    return groupValues;
}

void WaterQualityParameter::setGroupValues(const QList<double> &groupValues) {
    this->groupValues = groupValues;
}

bool WaterQualityParameter::isCheckable() const {
    return checkable;
}

void WaterQualityParameter::setCheckable(const bool &checkable) {
    this->checkable = checkable;
}

bool WaterQualityParameter::isChecked() const {
    return checked;
}

void WaterQualityParameter::setChecked(bool checked) {
    this->checked = checked;
}

void WaterQualityParameter::setEnabled(bool enabled) {
    this->enabled = enabled;
}

bool WaterQualityParameter::isEnabled() const {
    return enabled;
}

WaterQualityParameterType WaterQualityParameter::getType() const {
    return type;
}

void WaterQualityParameter::setType(const WaterQualityParameterType &type) {
    this->type = type;
}

WaterQualityParameterInputType WaterQualityParameter::getInputType() const {
    return inputType;
}

void WaterQualityParameter::setInputType(const WaterQualityParameterInputType &inputType) {
    this->inputType = inputType;
}

QString WaterQualityParameter::getLabel() const {
    return label;
}

void WaterQualityParameter::setLabel(const QString &label) {
    this->label = label;
}

QString WaterQualityParameter::getDescription() const {
    return description;
}

void WaterQualityParameter::setDescription(const QString &description) {
    this->description = description;
}

QString WaterQualityParameter::getDiagramItem() const {
    return diagramItem;
}

void WaterQualityParameter::setDiagramItem(const QString &diagramItem) {
    this->diagramItem = diagramItem;
}

WaterQualityParameterSection WaterQualityParameter::getSection() const {
    return section;
}

void WaterQualityParameter::setSection(const WaterQualityParameterSection &section) {
    this->section = section;
}

QString WaterQualityParameter::getTarget() const {
    return target;
}

void WaterQualityParameter::setTarget(const QString &target) {
    this->target = target;
}

QStringList WaterQualityParameter::getGroups() const {
    return groups;
}

void WaterQualityParameter::setGroups(const QStringList &groups) {
    this->groups = groups;
}

WaterQualityParameter* WaterQualityParameter::getParent() const {
    return parent;
}

void WaterQualityParameter::setParent(WaterQualityParameter *parent) {
    this->parent = parent;
    
    if (parent) {
        parent->children.append(this);
    }
}

bool WaterQualityParameter::isEditable() const {
    return editable;
}

void WaterQualityParameter::setEditable(bool editable) {
    this->editable = editable;
}

int WaterQualityParameter::getOrder() const {
    return order;
}

void WaterQualityParameter::setOrder(int order) {
    this->order = order;
}

double WaterQualityParameter::getRangeMinimum() const {
    return rangeMininum;
}

void WaterQualityParameter::setRangeMinimum(double rangeMininum) {
    this->rangeMininum = rangeMininum;
}

double WaterQualityParameter::getRangeMaximum() const {
    return rangeMaximum;
}

void WaterQualityParameter::setRangeMaximum(double rangeMaximum) {
    this->rangeMaximum = rangeMaximum;
}

QTreeWidgetItem* WaterQualityParameter::getItemWidget() const {
    return itemWidget;
}

void WaterQualityParameter::setItemWidget(QTreeWidgetItem *itemWidget) {
    this->itemWidget = itemWidget;
}

QList<WaterQualityParameter*> WaterQualityParameter::getSiblings() const {
    QList<WaterQualityParameter*> siblings;
    
    if (parent) {
        for (int i = 0; i < parent->children.size(); i++) {
            WaterQualityParameter *sibling = parent->children[i];
            
            if (sibling != this) {
                siblings.append(sibling);
            }
        }
    }
    
    return siblings;
}

void WaterQualityParameter::toggleSubTreeVisibility(bool hide) {
    this->itemWidget->setHidden(hide);
    
    for (int i = 0; i < children.size(); i++) {
        children[i]->toggleSubTreeVisibility(hide);
    }
}

QList<WaterQualityParameter*> WaterQualityParameter::getChildren() const {
    return children;
}

WaterQualityParameter* WaterQualityParameter::getChild(int i) const {
    return children[i];
}

SimulationDataType::WaterQualityParameter WaterQualityParameter::toSimulationDataType() const {
    SimulationDataType::WaterQualityParameter parameter;
    
    return parameter;
}

WaterQualityParameterType WaterQualityParameter::mapTypeFromString(const QString &typeStr) {
    if (typeStr == "initialCondition") {
        return WaterQualityParameterType::INITIAL_CONDITION;
    }
    
    return WaterQualityParameterType::PROCESS_INPUT;
}

QString WaterQualityParameter::mapStringFromType(const WaterQualityParameterType &type) {
    if (type == WaterQualityParameterType::INITIAL_CONDITION) {
        return "initialCondition";
    }
    
    return "processInput";
}

WaterQualityParameterInputType WaterQualityParameter::mapInputTypeFromString(const QString &inputTypeStr) {
    if (inputTypeStr == "inline") {
        return WaterQualityParameterInputType::INLINE;
    }
    
    if (inputTypeStr == "tabular") {
        return WaterQualityParameterInputType::TABULAR;
    }
    
    return WaterQualityParameterInputType::NO_INPUT;
}

QString WaterQualityParameter::mapStringFromInputType(const WaterQualityParameterInputType &inputType) {
    if (inputType == WaterQualityParameterInputType::INLINE) {
        return "inline";
    }
    
    if (inputType == WaterQualityParameterInputType::TABULAR) {
        return "tabular";
    }
    
    return "";
}

bool WaterQualityParameter::sort(WaterQualityParameter *p1, WaterQualityParameter *p2) {
    return p1->order < p2->order;
}