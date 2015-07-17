#include "include/repository/hydrodynamic_data_repository.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

HydrodynamicDataRepository::~HydrodynamicDataRepository() {
    for (int i = 0; i < parameters.size(); i++) {
        delete parameters[i];
    }
    for (int i = 0; i < processes.size(); i++) {
        delete processes[i];
    }
}

QList<HydrodynamicParameter*> HydrodynamicDataRepository::getParameters() {
    if (parameters.isEmpty()) {
        Project *project = IPHApplication::getCurrentProject();
        QFile dataFile(":/data/hydrodynamic_data.json");
    
        dataFile.open(QFile::ReadOnly);
        
        QJsonDocument jsonDocument = QJsonDocument::fromJson(dataFile.readAll());
        QJsonObject jsonObject = jsonDocument.object();
        QJsonArray jsonParameters = jsonObject["parameters"].toArray();
        
        for (int i = 0; i < jsonParameters.size(); i++) {
            QJsonObject jsonParameter = jsonParameters[i].toObject();
            HydrodynamicParameter *parameter = new HydrodynamicParameter();
            
            parameter->setName(jsonParameter["name"].toString());
            parameter->setLabel(jsonParameter["label"].toString());
            parameter->setEditable(jsonParameter["editable"].toBool());
            parameter->setSiblingsHidden(jsonParameter["hideSiblings"].toBool());
            parameter->setDefaultValue(jsonParameter["defaultValue"].toDouble());
            parameter->setRangeMinimum(jsonParameter["rangeMinimum"].toDouble());
            parameter->setRangeMaximum(jsonParameter["rangeMaximum"].toDouble());
            
            QString parentName = jsonParameter["parentName"].toString();
            HydrodynamicParameter *parentParameter = nullptr;
            
            if (!parentName.isEmpty()) {
                QList<HydrodynamicParameter*>::const_iterator it = parameters.constEnd();
                
                while (it != parameters.constBegin()) {
                    it--;
                    if ((*it)->getName() == parentName) {
                        parentParameter = *it;
                        break;
                    }
                }
            }
            
            parameter->setParent(parentParameter);
            parameters.append(parameter);
        }
        
        dataFile.close();
    }
    
    return parameters;
}

QList<HydrodynamicProcess*> HydrodynamicDataRepository::getProcesses() {
    if (processes.isEmpty()) {
        getParameters(); // Populate parameters list first
        
        QFile dataFile(":/data/hydrodynamic_data.json");
        
        dataFile.open(QFile::ReadOnly);
        
        QJsonDocument jsonDocument = QJsonDocument::fromJson(dataFile.readAll());
        QJsonObject jsonObject = jsonDocument.object();
        QJsonArray jsonProcesses = jsonObject["processes"].toArray();
        
        for (int i = 0; i < jsonProcesses.size(); i++) {
            QJsonObject jsonProcess = jsonProcesses[i].toObject();
            HydrodynamicProcess *process = new HydrodynamicProcess();
            
            process->setName(jsonProcess["name"].toString());
            process->setLabel(jsonProcess["label"].toString());
            process->setCheckable(jsonProcess["checkable"].toBool());
            process->setChecked(jsonProcess["checked"].toBool());
            
            HydrodynamicParameter *targetParameter = this->findParameterByName(jsonProcess["targetParameter"].toString());
            process->setTargetParameter(targetParameter);
            
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
        
        dataFile.close();
    }
    
    return processes;
}

template<typename T>
T* HydrodynamicDataRepository::findByName(const QList<T*> &list, const QString &name) const {
    for (int i = 0; i < list.size(); i++) {
        T *element = list[i];
        
        if (element->getName() == name) {
            return element;
        }
    }
    
    return nullptr;
}

HydrodynamicProcess* HydrodynamicDataRepository::findProcessByName(const QString &name) const {
    return findByName<HydrodynamicProcess>(processes, name);
}

HydrodynamicParameter* HydrodynamicDataRepository::findParameterByName(const QString &name) const {
    return findByName<HydrodynamicParameter>(parameters, name);
}