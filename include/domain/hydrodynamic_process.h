#ifndef HYDRODYNAMIC_PROCESS_H
#define HYDRODYNAMIC_PROCESS_H

#include "include/domain/hydrodynamic_parameter.h"
#include <QString>

class HydrodynamicProcess {
private:
    uint id;
    QString name;
    QString label;
    HydrodynamicProcess *parent;
    HydrodynamicParameter *targetParameter;
    bool checkable;
    bool checked;
    bool sibligsVisibleWhenChecked; // Remove?
public:
    HydrodynamicProcess(const QString &name, const QString &label, HydrodynamicProcess *parent = nullptr, bool checkable = false, HydrodynamicParameter *targetParameter = nullptr) :
        name(name), label(label), parent(parent), targetParameter(targetParameter), checkable(checkable), checked(false), sibligsVisibleWhenChecked(false) {}
    
    QString getName() const {
        return name;
    }
    
    void setName(const QString &name) {
        this->name = name;
    }

    QString getLabel() const {
        return label;
    }
    
    void setLabel(const QString &label) {
        this->label = label;
    }

    bool isCheckable() const {
        return checkable;
    }

    void setCheckable(bool checkable) {
        this->checkable = checkable;
    }

    bool isChecked() const {
        return checked;
    }

    bool setChecked(bool checked) {
        this->checked = checked;
    }

    HydrodynamicProcess* getParent() const {
        return parent;
    }

    bool isCheckableGroup() const {
        return checkable == true && targetParameter == nullptr;
    }
};

#endif // HYDRODYNAMIC_PROCESS_H
