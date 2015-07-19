#ifndef HYDRODYNAMIC_CONFIGURATION_H
#define HYDRODYNAMIC_CONFIGURATION_H

#include "include/domain/hydrodynamic_process.h"

#include <QString>
#include <QList>

class HydrodynamicConfiguration {
private:
	uint id;
	QString name;
	QList<HydrodynamicParameter*> parameters;
public:
    HydrodynamicConfiguration() : id(0) {}

	uint getId() const {
		return id;
	}

	void setId(uint id) {
		this->id = id;
	}

	bool isPersisted() const {
		return id != 0;
	}

	QString getName() const {
		return name;
	}

	void setName(const QString &name) {
		this->name = name;
	}
    
    void setParameters(const QList<HydrodynamicParameter*> &defaultParameters) {
        if (this->parameters.isEmpty()) {
            for (int i = 0; i < defaultParameters.size(); i++) {
                HydrodynamicParameter *defaultParameter = defaultParameters[i];
                HydrodynamicParameter *parameter = new HydrodynamicParameter();
                
                parameter->setName(defaultParameter->getName());
                parameter->setSelected(defaultParameter->isSelected());
                parameter->setValue(defaultParameter->getValue());
                parameter->setLabel(defaultParameter->getLabel());
                parameter->setRangeMinimum(defaultParameter->getRangeMinimum());
                parameter->setRangeMaximum(defaultParameter->getRangeMaximum());
                parameter->setProcess(defaultParameter->getProcess());
                parameter->setItemWidget(defaultParameter->getItemWidget());
                
                this->parameters.append(parameter);
            }
        }
    }
    
	bool addHydrodynamicParameter(HydrodynamicParameter *hydrodynamicParameter) {
		if (parameters.contains(hydrodynamicParameter)) {
			return false;
		}

		parameters.append(hydrodynamicParameter);

		return true;
	}

	QList<HydrodynamicParameter*> getParameters() const {
		return parameters;
	}

	HydrodynamicParameter* getParameter(const QString &name) {
		for (int i = 0; i < parameters.size(); i++) {
			if (parameters[i]->getName() == name) {
				return parameters[i];
			}
		}

		return nullptr;
	}
};

#endif // HYDRODYNAMIC_CONFIGURATION_H