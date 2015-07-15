#ifndef HYDRODYNAMIC_PARAMETER_H
#define HYDRODYNAMIC_PARAMETER_H

#include <QString>

class HydrodynamicParameter {
private:
    uint id;
    QString name;
    QString label;
    HydrodynamicParameter *parent;
    bool editable;
    double defaultValue;
    double value;
    double rangeMininum;
    double rangeMaximum;
public:
    HydrodynamicParameter(const QString &name, const QString &label, HydrodynamicParameter *parent = nullptr, bool editable = true) :
        name(name), label(label), parent(parent), editable(editable) {}
    
    QString getName() const {
        return name;
    }
    
    void setName(const QString& name) {
        this->name = name;
    }

    QString getLabel() const {
        return label;
    }
    
    void setLabel(const QString &label) {
        this->label = label;
    }
    
    double getValue() const {
        return value;
    }
    
    void setValue(double value) {
        this->value = value;
    }
    
    bool isEditable() const {
        return editable;
    }
    
    void setEditable(bool editable) {
        this->editable = editable;
    }

    HydrodynamicParameter* getParent() const {
        return parent;
    }
};

#endif // HYDRODYNAMIC_PARAMETER_H
