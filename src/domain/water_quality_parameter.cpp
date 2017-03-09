#include <domain/water_quality_parameter.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

WaterQualityParameter::WaterQualityParameter() :
	id(0),
    checkable(false),
    checked(false),
    enabled(true),
    target(nullptr),
    source(nullptr),
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

QMap<QString, QList<double> > WaterQualityParameter::getGroupValues() const {
    return groupValues;
}

void WaterQualityParameter::setGroupValues(const QMap<QString, QList<double> > &groupValues) {
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

void WaterQualityParameter::setInputTypeStr(const QString &inputTypeStr) {
    if (inputTypeStr.toLower() == "inline") {
        this->inputType = WaterQualityParameterInputType::INLINE;
    } else if (inputTypeStr.toLower() == "tabular") {
        this->inputType = WaterQualityParameterInputType::TABULAR;
    } else {
        this->inputType = WaterQualityParameterInputType::NO_INPUT;
    }
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
    
    if (target) {
        target->setSource(this);
    }
}

WaterQualityParameter* WaterQualityParameter::getSource() const {
    return source;
}

void WaterQualityParameter::setSource(WaterQualityParameter *source) {
    this->source = source;
}

QString WaterQualityParameter::getGroupValuesStr() const {
    QJsonDocument jsonDocument;
    QJsonObject jsonObject;
    
    for (QMap<QString, QList<double> >::const_iterator it = groupValues.begin(); it != groupValues.end(); it++) {
        QJsonArray jsonArray;
        
        for (double value : it.value()) {
            jsonArray.append(value);
        }
        
        jsonObject.insert(it.key(), jsonArray);
    }
    
    return QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);
}

void WaterQualityParameter::setGroupValues(const QString &groupValuesStr) {
    QJsonDocument jsonDocument = QJsonDocument::fromJson(groupValuesStr.toLocal8Bit());
    QJsonObject jsonObject(jsonDocument.object());
    
    for (QJsonObject::const_iterator it = jsonObject.begin(); it != jsonObject.end(); it++) {
        QJsonArray jsonArray = it.value().toArray();
        QList<double> values;
        
        for (int i = 0; i < jsonArray.size(); i++) {
            values.append(jsonArray.at(i).toDouble());
        }
        
        groupValues.insert(it.key(), values);
    }
}

void WaterQualityParameter::clearGroupValues() {
    groupValues.clear();
}

QStringList WaterQualityParameter::getGroups() const {
    return groups;
}

void WaterQualityParameter::setGroups(const QList<WaterQualityParameter*> &groups) {
    for (WaterQualityParameter *group : groups) {
        this->groups.append(group->getName());
        group->groupTargets.append(this);
    }
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

QMap<QString, double> WaterQualityParameter::getDefaultGroupValues() const {
    return defaultGroupValues;
}

void WaterQualityParameter::setDefaultGroupValues(const QMap<QString, double> &defaultGroupValues) {
    this->defaultGroupValues = defaultGroupValues;
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

QList<WaterQualityParameter*> WaterQualityParameter::getChildren() const {
    return children;
}

WaterQualityParameter* WaterQualityParameter::getChild(int i) const {
    return children[i];
}

QList<WaterQualityParameter*> WaterQualityParameter::getGroupTargets() const {
    return groupTargets;
}

bool WaterQualityParameter::sort(WaterQualityParameter *p1, WaterQualityParameter *p2) {
    return p1->order < p2->order;
}

SimulationDataType::WaterQualityParameter WaterQualityParameter::toSimulationDataType() const {
    SimulationDataType::WaterQualityParameter parameter;
    QByteArray nameByteArray = this->name.toLocal8Bit();
    
    strncpy(parameter.name, nameByteArray.constData(), nameByteArray.length());
    
    parameter.parameterType = (int) section;
    
    if (section == WaterQualityParameterSection::STRUCTURE || inputType == WaterQualityParameterInputType::INLINE) {
        parameter.numberOfGroups = 0;
        parameter.value = section == WaterQualityParameterSection::STRUCTURE ? this->checked : this->value;
    } else { // WaterQualityParameterInputType::TABULAR
        int i = 0;
        
        parameter.value = std::numeric_limits<double>::min();
        parameter.numberOfGroups = groupValues.size();
        parameter.groups = new SimulationDataType::WaterQualityGroup[parameter.numberOfGroups];
        
        for (QMap<QString, QList<double> >::const_iterator it = groupValues.begin(); it != groupValues.end(); it++) {
            QByteArray nameByteArray = it.key().toLocal8Bit();
            
            strncpy(parameter.groups[i].name, nameByteArray.constData(), nameByteArray.length());
            parameter.groups[i].numberOfValues = it.value().size();
            parameter.groups[i].values = new double[parameter.groups[i].numberOfValues];
            
            for (int j = 0; j < parameter.groups[i].numberOfValues; j++) {
                parameter.groups[i].values[j] = it.value().at(j);
            }
            
            i++;
        }
    }
    
    return parameter;
}
