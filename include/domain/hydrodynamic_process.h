#ifndef HYDRODYNAMIC_PROCESS_H
#define HYDRODYNAMIC_PROCESS_H

#include <domain/hydrodynamic_parameter.h>
#include <QTreeWidgetItem>
#include <QString>
#include <QList>

class HydrodynamicProcess {
private:
    QString name;
    QString label;
    HydrodynamicProcess *parent;
    HydrodynamicParameter *targetParameter;
    bool checkable;
    bool checked;
    QList<HydrodynamicProcess*> children;
    QTreeWidgetItem *itemWidget;
public:
    HydrodynamicProcess();
    
    QString getName() const;
    void setName(const QString &name);
    QString getLabel() const;
    void setLabel(const QString &label);
    bool isCheckable() const;
    void setCheckable(bool checkable);
    bool isChecked() const;
    void setChecked(bool checked);
    HydrodynamicProcess* getParent() const;
    void setParent(HydrodynamicProcess *parent);
    HydrodynamicParameter* getTargetParameter() const;
    void setTargetParameter(HydrodynamicParameter *targetParameter);
    bool isCheckableGroup() const;
    QTreeWidgetItem* getItemWidget() const;
    void setItemWidget(QTreeWidgetItem *itemWidget);
    QList<HydrodynamicProcess*> getChildren() const;
    QList<HydrodynamicProcess*> getSiblings() const;
    bool hasChildChecked() const;
    bool hasSiblingChecked() const;
    void uncheckChildren();
    void uncheckSiblings();
};

#endif // HYDRODYNAMIC_PROCESS_H
