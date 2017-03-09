#ifndef WATER_QUALITY_PARAMETER_H
#define WATER_QUALITY_PARAMETER_H

#include "simulation_data_type.h"

#include <QTreeWidgetItem>
#include <QString>
#include <QList>
#include <QMap>

enum class WaterQualityParameterInputType { NO_INPUT = 0, INLINE, TABULAR };
enum class WaterQualityParameterSection { STRUCTURE = 1, PARAMETER, INITIAL_CONDITION };

class WaterQualityParameter {
private:
    uint id;
    QString name;
    double value;
    QMap<QString, QList<double> > groupValues;
    bool persistable;
    bool checkable;
    bool checked;
    bool enabled;
    WaterQualityParameterInputType inputType;
    
    // Trasient attributes
    QString label;
    QString description;
    QString diagramItem;
    WaterQualityParameterSection section;
    WaterQualityParameter *target;
    QList<WaterQualityParameter*> groupTargets;
    QStringList groups;
    WaterQualityParameter *parent;
    QMap<QString, double> defaultGroupValues;
    bool editable;
    bool radio;
    bool group;
    int order;
    double rangeMininum;
    double rangeMaximum;
    QList<WaterQualityParameter*> children;
    QTreeWidgetItem* itemWidget;
public:
    WaterQualityParameter();
    WaterQualityParameter(WaterQualityParameter *parameter);

    uint getId() const;
    void setId(const uint &id);
    bool isPersisted() const;
    QString getName() const;
    void setName(const QString &name);
    double getValue() const;
    void setValue(double value);
    QString getGroupValuesStr() const;
    void setGroupValues(const QString &groupValuesStr);
    QMap<QString, QList<double> > getGroupValues() const;
    void setGroupValues(const QMap<QString, QList<double> > &groupValues);
    void clearGroupValues();
    bool isPersistable() const;
    void setPersistable(const bool &persistable);
    bool isCheckable() const;
    void setCheckable(const bool &checkable);
    bool isChecked() const;
    void setChecked(bool checked);
    void setEnabled(bool enabled);
    bool isEnabled() const;
    WaterQualityParameterInputType getInputType() const;
    void setInputTypeStr(const QString &inputType);
    void setInputType(const WaterQualityParameterInputType &inputType);
    
    QString getLabel() const;
    void setLabel(const QString &label);
    QString getDescription() const;
    void setDescription(const QString &description);
    QString getDiagramItem() const;
    void setDiagramItem(const QString &diagramItem);
    WaterQualityParameterSection getSection() const;
    void setSection(const WaterQualityParameterSection &section);
    WaterQualityParameter* getTarget() const;
    void setTarget(WaterQualityParameter *target);
    QStringList getGroups() const;
    void setGroups(const QList<WaterQualityParameter*> &groups);
    WaterQualityParameter* getParent() const;
    void setParent(WaterQualityParameter *parent);
    QMap<QString, double> getDefaultGroupValues() const;
    void setDefaultGroupValues(const QMap<QString, double> &defaultGroupValues);
    bool isEditable() const;
    void setEditable(bool editable);
    bool isRadio() const;
    void setRadio(const bool &radio);
    bool isGroup() const;
    void setGroup(const bool &group);
    int getOrder() const;
    void setOrder(int order);
    double getRangeMinimum() const;
    void setRangeMinimum(double rangeMininum);
    double getRangeMaximum() const;
    void setRangeMaximum(double rangeMaximum);
    QTreeWidgetItem* getItemWidget() const;
    void setItemWidget(QTreeWidgetItem *itemWidget);
    
    QList<WaterQualityParameter*> getSiblings() const;
    QList<WaterQualityParameter*> getChildren() const;
    WaterQualityParameter* getChild(int i) const;
    QList<WaterQualityParameter*> getGroupTargets() const;
    SimulationDataType::WaterQualityParameter toSimulationDataType() const;
    
    static bool sort(WaterQualityParameter *p1, WaterQualityParameter *p2);
    
    inline bool isInRange(double value) const {
        return (rangeMininum == 0 && rangeMaximum == 0) || (value >= rangeMininum && value <= rangeMaximum);
    }
    
    inline bool operator==(const WaterQualityParameter &parameter) const {
        return this->name == parameter.getName() && this->section == parameter.getSection();
    }
};

#endif // WATER_QUALITY_PARAMETER_H
