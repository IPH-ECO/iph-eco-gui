#ifndef WATER_QUALITY_PARAMETER_H
#define WATER_QUALITY_PARAMETER_H

#include "simulation_data_type.h"

#include <QTreeWidgetItem>
#include <QString>
#include <QList>

enum class WaterQualityParameterType { PROCESS_INPUT = 1, INITIAL_CONDITION };
enum class WaterQualityParameterInputType { NO_INPUT = 0, INLINE, TABULAR };

class WaterQualityParameter {
private:
    uint id;
    QString name;
    double value;
    QList<double> groupValues;
    bool checkable;
    bool checked;
    bool enabled;
    WaterQualityParameterType type;
    WaterQualityParameterInputType inputType;
    
    // Trasient attributes
    QString label;
    QString description;
    QString diagramItem;
    QStringList groups;
    WaterQualityParameter *parent;
    bool editable;
    int order;
    double rangeMininum;
    double rangeMaximum;
    QList<WaterQualityParameter*> children;
    QTreeWidgetItem* itemWidget;
public:
    WaterQualityParameter();

    uint getId() const;
    void setId(const uint &id);
    bool isPersisted() const;
    QString getName() const;
    void setName(const QString &name);
    double getValue() const;
    void setValue(double value);
    QList<double> getGroupValues() const;
    void setGroupValues(const QList<double> &groupValues);
    bool isCheckable() const;
    void setCheckable(const bool &checkable);
    bool isChecked() const;
    void setChecked(bool checked);
    void setEnabled(bool enabled);
    bool isEnabled() const;
    WaterQualityParameterType getType() const;
    void setType(const WaterQualityParameterType &type);
    WaterQualityParameterInputType getInputType() const;
    void setInputType(const WaterQualityParameterInputType &inputType);

    QString getLabel() const;
    void setLabel(const QString &label);
    QString getDescription() const;
    void setDescription(const QString &description);
    QString getDiagramItem() const;
    void setDiagramItem(const QString &diagramItem);
    QStringList getGroups() const;
    void setGroups(const QStringList &groups);
    WaterQualityParameter* getParent() const;
    void setParent(WaterQualityParameter *parent);
    bool isEditable() const;
    void setEditable(bool editable);
    int getOrder() const;
    void setOrder(int order);
    double getRangeMinimum() const;
    void setRangeMinimum(double rangeMininum);
    double getRangeMaximum() const;
    void setRangeMaximum(double rangeMaximum);
    QTreeWidgetItem* getItemWidget() const;
    void setItemWidget(QTreeWidgetItem *itemWidget);
    
    QList<WaterQualityParameter*> getSiblings() const;
    void toggleSubTreeVisibility(bool hide);
    QList<WaterQualityParameter*> getChildren() const;
    WaterQualityParameter* getChild(int i) const;
    SimulationDataType::WaterQualityParameter toSimulationDataType() const;
    
    static WaterQualityParameterType mapTypeFromString(const QString &typeStr);
    static QString mapStringFromType(const WaterQualityParameterType &type);
    static WaterQualityParameterInputType mapInputTypeFromString(const QString &inputTypeStr);
    static QString mapStringFromInputType(const WaterQualityParameterInputType &inputType);
    static bool sort(WaterQualityParameter *p1, WaterQualityParameter *p2);
    
    inline bool isProcessInput() const {
        return type == WaterQualityParameterType::PROCESS_INPUT;
    }
    
    inline bool isInitialCondition() const {
        return type == WaterQualityParameterType::INITIAL_CONDITION;
    }
    
    inline bool isInRange(double value) const {
        return (rangeMininum == 0 && rangeMaximum == 0) || (value >= rangeMininum && value <= rangeMaximum);
    }
};

#endif // WATER_QUALITY_PARAMETER_H
