#include <repository/water_quality_repository.h>

#include <domain/water_quality_parameter.h>

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

WaterQualityRepository* WaterQualityRepository::instance = nullptr;

WaterQualityRepository* WaterQualityRepository::getInstance() {
    if (!instance) {
        instance = new WaterQualityRepository();
    }
    
    return instance;
}

WaterQualityRepository::WaterQualityRepository() {
    QFile structureFile(":/data/water_quality_structure.json");

    structureFile.open(QFile::ReadOnly);

    QJsonDocument jsonDocument = QJsonDocument::fromJson(structureFile.readAll());
    QJsonArray jsonParameters = jsonDocument.array();
    
    structureFile.close();

    for (int i = 0; i < jsonParameters.size(); i++) {
        QJsonObject jsonParameter = jsonParameters[i].toObject();
        WaterQualityParameter *parameter = new WaterQualityParameter();
        
        parameter->setName(jsonParameter["name"].toString());
        parameter->setLabel(jsonParameter["label"].toString());
        parameter->setDescription(jsonParameter["label"].toString());
        parameter->setSection(WaterQualityParameterSection::STRUCTURE);
        parameter->setTarget(jsonParameter["target"].toString());
        parameter->setDiagramItem(jsonParameter["diagramItem"].toString());
        parameter->setCheckable(jsonParameter["checkable"].toBool(false));
        parameter->setChecked(jsonParameter["checked"].toBool());
        
        parameters.append(parameter);
        
        QString parentName = jsonParameter["parentName"].toString();
        
        for (WaterQualityParameter *parent : parameters) {
            if (parent->getName() == parentName) {
                parameter->setParent(parent);
                break;
            }
        }
        
        if (!parameter->getParent()) {
            rootParameters.append(parameter);
        }
    }
    
    QFile parametersFile(":/data/water_quality_parameters.json");
    
    parametersFile.open(QFile::ReadOnly);
    
    jsonDocument = QJsonDocument::fromJson(parametersFile.readAll());
    jsonParameters = jsonDocument.array();
    WaterQualityParameter *lastGroupParameter = nullptr;
    QStringList lastGroups;
    
    parametersFile.close();
    
    for (int i = 0; i < jsonParameters.size(); i++) {
        QJsonObject jsonParameter = jsonParameters[i].toObject();
        WaterQualityParameter *parameter = new WaterQualityParameter();
        
        parameter->setName(jsonParameter["name"].toString());
        parameter->setLabel(jsonParameter["label"].toString());
        parameter->setSection(WaterQualityParameterSection::PARAMETER);
        parameter->setTarget(jsonParameter["target"].toString());
        parameter->setDescription(jsonParameter["description"].toString());
        parameter->setEnabled(jsonParameter["enabled"].toBool(true));
        parameter->setInputType(WaterQualityParameter::mapInputTypeFromString(jsonParameter["inputType"].toString()));
        parameter->setRangeMinimum(jsonParameter["rangeMinimum"].toDouble());
        parameter->setRangeMaximum(jsonParameter["rangeMaximum"].toDouble());
        parameter->setValue(jsonParameter["defaultValue"].toDouble());
        
        if (parameter->getLabel() == "Groups") {
            lastGroupParameter = parameter;
            lastGroups.clear();
        }
        
        QString parentName = jsonParameter["parentName"].toString();
        
        if (lastGroupParameter) {
            if (lastGroupParameter->getName() == parentName) {
                lastGroups.append(parameter->getName());
            } else {
                QString group = jsonParameter["groups"].toString();
                
                if (lastGroupParameter->getName() == group) {
                    parameter->setGroups(lastGroups);
                }
            }
        }
        
        if (!jsonParameter["groupDefaultValues"].isUndefined()) {
            if (jsonParameter["groupDefaultValues"].isArray()) {
                QJsonArray defaultValuesArray = jsonParameter["groupDefaultValues"].toArray();
                QList<double> values;

                for (QJsonValue value : defaultValuesArray) {
                    values.append(value.toDouble());
                }

                parameter->setGroupValues(values);
            } else {
                parameter->setGroupValues(QVector<double>(lastGroups.size(), jsonParameter["groupDefaultValues"].toDouble()).toList());
            }
        }
        
        for (WaterQualityParameter *parentParameter : parameters) {
            if (parentParameter->getName() == parentName) {
                parameter->setParent(parentParameter);
                break;
            }
        }
        
        if (!parameter->getParent()) {
            rootParameters.append(parameter);
        }
        
        parameters.append(parameter);
    }
}

void WaterQualityRepository::buildParameters(WaterQualityConfiguration *configuration) {
	
}

WaterQualityParameter* WaterQualityRepository::findParameterByName(const QString &name, const WaterQualityParameterSection &section) {
    for (WaterQualityParameter *parameter : parameters) {
        if (parameter->getName() == name && parameter->getSection() == section) {
            return parameter;
        }
    }
    
    return nullptr;
}

QList<WaterQualityParameter*> WaterQualityRepository::getParameters() const {
    return parameters;
}

QList<WaterQualityParameter*> WaterQualityRepository::getRootParameters() const {
    return rootParameters;
}