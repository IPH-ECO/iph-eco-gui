#include <domain/water_quality_parameter.h>

WaterQualityParameter::WaterQualityParameter() :
	id(0),
    checkable(false),
    checked(false),
    enabled(true),
    parent(nullptr),
    editable(true),
    group(false),
    rangeMininum(0),
    rangeMaximum(0),
    itemWidget(nullptr)
{}

WaterQualityParameter::WaterQualityParameter(WaterQualityParameter *parameter) :
    id(0),
    name(parameter->getName()),
    value(parameter->getValue()),
    groupValues(parameter->getGroupValues())
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

bool WaterQualityParameter::isPersistable() const {
    return persistable;
}

void WaterQualityParameter::setPersistable(const bool &persistable) {
    this->persistable = persistable;
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

WaterQualityParameter* WaterQualityParameter::getTarget() const {
    return target;
}

void WaterQualityParameter::setTarget(WaterQualityParameter *target) {
    this->target = target;
}

QString WaterQualityParameter::getGroupValuesStr() const {
    if (groupValues.isEmpty()) {
        return "";
    }
    
    QStringList jsonGroupValues;
    
    for (double groupValue : groupValues) {
        jsonGroupValues.append(QString::number(groupValue));
    }
    
    return jsonGroupValues.join(",");
}

void WaterQualityParameter::setGroupValues(const QString &groupValuesStr) {
    for (QString groupValueStr : groupValuesStr.split(",")) {
        groupValues.append(groupValueStr.toDouble());
    }
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

bool WaterQualityParameter::isRadio() const {
    return radio;
}

void WaterQualityParameter::setRadio(const bool &radio) {
    this->radio = radio;
}

bool WaterQualityParameter::isGroup() const {
    return group;
}

void WaterQualityParameter::setGroup(const bool &group) {
    this->group = group;
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