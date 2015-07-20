#include "include/repository/hydrodynamic_data_repository.h"
#include "include/application/iph_application.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

HydrodynamicDataRepository* HydrodynamicDataRepository::instance = nullptr;

HydrodynamicDataRepository* HydrodynamicDataRepository::getInstance() {
    if (instance == nullptr) {
        instance = new HydrodynamicDataRepository();
    }
    
    return instance;
}

HydrodynamicDataRepository::HydrodynamicDataRepository() {
    QFile dataFile(":/data/hydrodynamic_data.json");
    
    dataFile.open(QFile::ReadOnly);
    
    QJsonDocument jsonDocument = QJsonDocument::fromJson(dataFile.readAll());
    QJsonObject jsonObject = jsonDocument.object();
    
    dataFile.close();
    
    this->jsonParameters = jsonObject["parameters"].toArray();
    this->jsonProcesses = jsonObject["processes"].toArray();
    
    for (int i = 0; i < jsonProcesses.size(); i++) {
        QJsonObject jsonProcess = jsonProcesses[i].toObject();
        HydrodynamicProcess *process = new HydrodynamicProcess();
        
        process->setName(jsonProcess["name"].toString());
        process->setLabel(jsonProcess["label"].toString());
        process->setCheckable(jsonProcess["checkable"].toBool());
        
        QString parentName = jsonProcess["parentName"].toString();
        HydrodynamicProcess *parentProcess = nullptr;
        
        if (!parentName.isEmpty()) {
            QList<HydrodynamicProcess*>::const_iterator it = processes.constEnd();
            
            while (it != processes.constBegin()) {
                it--;
                if ((*it)->getName() == parentName) {
                    parentProcess = *it;
                    break;
                }
            }
        }
        
        process->setParent(parentProcess);
        processes.append(process);
    }
}

HydrodynamicDataRepository::~HydrodynamicDataRepository() {}

void HydrodynamicDataRepository::buildParameters(HydrodynamicConfiguration *configuration) {
    for (int i = 0; i < jsonParameters.size(); i++) {
        QJsonObject jsonParameter = jsonParameters[i].toObject();
        QString parameterName = jsonParameter["name"].toString();
        HydrodynamicParameter *parameter = configuration->getParameter(parameterName);

        if (parameter == nullptr) {
            parameter = new HydrodynamicParameter();
            parameter->setValue(jsonParameter["defaultValue"].toDouble());
            parameter->setSelected(jsonParameter["selected"].toBool());
        }

        parameter->setName(parameterName);
        parameter->setLabel(jsonParameter["label"].toString());
        parameter->setEditable(jsonParameter["editable"].toBool());
        parameter->setPersistable(jsonParameter["persistable"].toBool());
        parameter->setSiblingsHidden(jsonParameter["hideSiblings"].toBool());
        parameter->setOrder(jsonParameter["order"].toInt(-1));
        parameter->setRangeMinimum(jsonParameter["rangeMinimum"].toDouble());
        parameter->setRangeMaximum(jsonParameter["rangeMaximum"].toDouble());
        
        if (!parameter->getParent()) {
            HydrodynamicParameter *parentParameter = configuration->getParameter(jsonParameter["parentName"].toString());
            parameter->setParent(parentParameter);
        }

        configuration->addHydrodynamicParameter(parameter);
    }
}

QList<HydrodynamicProcess*> HydrodynamicDataRepository::getProcesses(HydrodynamicConfiguration *configuration) {
    for (int i = 0; i < jsonProcesses.size(); i++) {
        QJsonObject jsonProcess = jsonProcesses[i].toObject();
        HydrodynamicProcess *process = processes[i];
        HydrodynamicParameter *targetParameter = configuration->getParameter(jsonProcess["targetParameter"].toString());

        process->setTargetParameter(targetParameter);
        
        if (targetParameter != nullptr) {
            process->setChecked(targetParameter->isSelected());
        }
    }
    
    return processes;
}

HydrodynamicProcess* HydrodynamicDataRepository::findProcessByName(const QString &name) const {
    for (int i = 0; i < processes.size(); i++) {
        HydrodynamicProcess *process = processes[i];
        
        if (process->getName() == name) {
            return process;
        }
    }
    
    return nullptr;
}