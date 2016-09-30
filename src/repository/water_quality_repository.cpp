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
    QJsonDocument jsonDocument;
    
    structureFile.open(QFile::ReadOnly);
    jsonDocument = QJsonDocument::fromJson(structureFile.readAll());
    this->jsonStructure = jsonDocument.array();
    structureFile.close();

    QFile parametersFile(":/data/water_quality_parameters.json");
    parametersFile.open(QFile::ReadOnly);
    jsonDocument = QJsonDocument::fromJson(parametersFile.readAll());
    this->jsonParameters = jsonDocument.array();
    parametersFile.close();
    
    QFile foodMatrixFile(":/data/food_matrix.json");
    foodMatrixFile.open(QFile::ReadOnly);
    jsonDocument = QJsonDocument::fromJson(foodMatrixFile.readAll());
    this->jsonFoodMatrix = jsonDocument.array();
    foodMatrixFile.close();
    
    QFile initialConditionsFile(":/data/water_quality_initial_conditions.json");
    initialConditionsFile.open(QFile::ReadOnly);
    jsonDocument = QJsonDocument::fromJson(initialConditionsFile.readAll());
    this->jsonInitialConditions = jsonDocument.array();
    initialConditionsFile.close();
    
    for (int i = 0; i < jsonFoodMatrix.size(); i++) {
        QJsonObject jsonFood = jsonFoodMatrix[i].toObject();
        FoodMatrixElement *foodMatrixElement = new FoodMatrixElement();
        
        foodMatrixElement->setName(jsonFood["name"].toString());
        foodMatrixElement->setLabel(jsonFood["label"].toString());
        
        if (jsonFood["prey"].toBool()) {
            preys.append(foodMatrixElement);
        }
        
        if (jsonFood["predator"].toBool()) {
            QJsonArray jsonPredatorValues = jsonFood["values"].toArray();
            
            for (int j = 0; j < jsonPredatorValues.size(); j++) {
                QJsonObject jsonPreyValuePair = jsonPredatorValues[j].toObject();
                FoodMatrixElement *prey = this->findPreyByName(jsonPreyValuePair["prey"].toString());
                FoodMatrixElement *predator = foodMatrixElement;
                
                predator->addPrey(prey);
                defaultFoodMatrix.append(FoodMatrixItem(predator, prey, jsonPreyValuePair["value"].toDouble()));
            }
            
            predators.append(foodMatrixElement);
        }
    }
}

void WaterQualityRepository::loadParameters(WaterQualityConfiguration *configuration) {
    if (configuration->isLoaded()) {
        return;
    }
    
    WaterQualityParameter *lastGroupParameter = nullptr;
    QStringList lastGroups;
    
    for (int i = 0; i < this->jsonParameters.size(); i++) {
        QJsonObject jsonParameter = jsonParameters[i].toObject();
        QString parameterName = jsonParameter["name"].toString();
        WaterQualityParameter *parameter = configuration->getParameter(parameterName, WaterQualityParameterSection::PARAMETER);
        bool overrideGroupDefaultValues = false;
        
        if (!parameter) {
            parameter = new WaterQualityParameter();
            parameter->setValue(jsonParameter["defaultValue"].toDouble());
            overrideGroupDefaultValues = true;
        }
        
        parameter->setName(jsonParameter["name"].toString());
        parameter->setLabel(jsonParameter["label"].toString());
        parameter->setSection(WaterQualityParameterSection::PARAMETER);
        parameter->setDescription(jsonParameter["description"].toString());
        parameter->setEnabled(jsonParameter["enabled"].toBool(true));
        parameter->setInputTypeStr(jsonParameter["inputType"].toString());
        parameter->setRangeMinimum(jsonParameter["rangeMinimum"].toDouble());
        parameter->setRangeMaximum(jsonParameter["rangeMaximum"].toDouble());
        parameter->setRadio(jsonParameter["radio"].toBool(false));
        parameter->setGroup(jsonParameter["group"].toBool(false));
        parameter->setPersistable(jsonParameter["persistable"].toBool(true));
        
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
        
        if (!jsonParameter["groupDefaultValues"].isNull()) {
            QMap<QString, double> defaultGroupValues;
            
            if (jsonParameter["groupDefaultValues"].isArray()) {
                QJsonArray defaultValuesArray = jsonParameter["groupDefaultValues"].toArray();
                
                for (int i = 0; i < lastGroups.size(); i++) {
                    defaultGroupValues.insert(lastGroups.at(i), defaultValuesArray.at(i).toDouble());
                }
            } else {
                for (int i = 0; i < lastGroups.size(); i++) {
                    defaultGroupValues.insert(lastGroups.at(i), jsonParameter["groupDefaultValues"].toDouble());
                }
            }
            
            parameter->setDefaultGroupValues(defaultGroupValues);
            
            if (overrideGroupDefaultValues) {
                QMap<QString, QList<double> > groupValues;
                
                for (QMap<QString, double>::const_iterator it = defaultGroupValues.begin(); it != defaultGroupValues.end(); it++) {
                    WaterQualityParameter *groupParameter = configuration->getParameter(it.key(), WaterQualityParameterSection::PARAMETER);
                    int groupCount = groupParameter->getValue();
                    
                    if (groupCount > 0) {
                        groupValues.insert(it.key(), QVector<double>(groupCount, it.value()).toList());
                    }
                }
                
                parameter->setGroupValues(groupValues);
            }
        }
        
        WaterQualityParameter *parentParameter = configuration->getParameter(parentName, WaterQualityParameterSection::PARAMETER);
        
        parameter->setParent(parentParameter);
        configuration->addWaterQualityParameter(parameter);
    }
    
    for (int i = 0; i < this->jsonStructure.size(); i++) {
        QJsonObject jsonParameter = jsonStructure[i].toObject();
        QString parameterName = jsonParameter["name"].toString();
        WaterQualityParameter *parameter = configuration->getParameter(parameterName, WaterQualityParameterSection::STRUCTURE);
        WaterQualityParameter *targetParameter = configuration->getParameter(jsonParameter["target"].toString(), WaterQualityParameterSection::PARAMETER);
        
        if (parameter) {
            parameter->setChecked(parameter->getValue());
        } else {
            parameter = new WaterQualityParameter();
            parameter->setChecked(jsonParameter["checked"].toBool());
            parameter->setValue(parameter->isChecked());
        }
        
        parameter->setName(jsonParameter["name"].toString());
        parameter->setLabel(jsonParameter["label"].toString());
        parameter->setDescription(jsonParameter["label"].toString());
        parameter->setSection(WaterQualityParameterSection::STRUCTURE);
        parameter->setTarget(targetParameter);
        parameter->setDiagramItem(jsonParameter["diagramItem"].toString());
        parameter->setCheckable(jsonParameter["checkable"].toBool(false));
        parameter->setEnabled(jsonParameter["enabled"].toBool(true));
        parameter->setRadio(jsonParameter["radio"].toBool(false));
        parameter->setGroup(jsonParameter["group"].toBool(false));
        parameter->setPersistable(jsonParameter["persistable"].toBool(true));
        parameter->setInputType(WaterQualityParameterInputType::NO_INPUT);
        
        WaterQualityParameter *parentParameter = configuration->getParameter(jsonParameter["parentName"].toString(), WaterQualityParameterSection::STRUCTURE);
        
        parameter->setParent(parentParameter);
        configuration->addWaterQualityParameter(parameter);
    }
    
    for (int i = 0; i < jsonFoodMatrix.size(); i++) {
        QJsonObject jsonFood = jsonFoodMatrix[i].toObject();
        QString elementName = jsonFood["name"].toString();
        FoodMatrixElement *foodMatrixElement = nullptr;
        
        for (FoodMatrixElement *predator : predators) {
            if (predator->getName() == elementName) {
                foodMatrixElement = predator;
                break;
            }
        }
        
        if (!foodMatrixElement) {
            for (FoodMatrixElement *prey : preys) {
                if (prey->getName() == elementName) {
                    foodMatrixElement = prey;
                    break;
                }
            }
        }
        
        WaterQualityParameter *parameter = configuration->getParameter(jsonFood["parameter"].toString(), WaterQualityParameterSection::PARAMETER);
        WaterQualityParameter *group = configuration->getParameter(jsonFood["group"].toString(), WaterQualityParameterSection::PARAMETER);
        
        foodMatrixElement->setGroup(group);
        foodMatrixElement->setParameter(parameter);
    }
    
    for (int i = 0; i < this->jsonInitialConditions.size(); i++) {
        QJsonObject jsonParameter = this->jsonInitialConditions[i].toObject();
        QString parameterName = jsonParameter["name"].toString();
        QStringList groups = this->findGroups(configuration, jsonParameter["groups"].toString());
        WaterQualityParameter *parameter = configuration->getParameter(parameterName, WaterQualityParameterSection::INITIAL_CONDITION);
        WaterQualityParameterSection targetParameterSection = jsonParameter["section"].toString() == "parameter" ? WaterQualityParameterSection::PARAMETER : WaterQualityParameterSection::STRUCTURE;
        WaterQualityParameter *targetParameter = configuration->getParameter(jsonParameter["target"].toString(), targetParameterSection);
        bool overrideGroupDefaultValues = false;
        
        if (!parameter) {
            parameter = new WaterQualityParameter();
            parameter->setValue(jsonParameter["defaultValue"].toDouble());
            overrideGroupDefaultValues = true;
        }
        
        parameter->setName(jsonParameter["name"].toString());
        parameter->setLabel(jsonParameter["label"].toString());
        parameter->setSection(WaterQualityParameterSection::INITIAL_CONDITION);
        parameter->setTarget(targetParameter);
        parameter->setDescription(jsonParameter["description"].toString());
        parameter->setInputTypeStr(jsonParameter["inputType"].toString());
        parameter->setPersistable(parameter->getInputType() != WaterQualityParameterInputType::NO_INPUT);
        parameter->setGroups(groups);
        
        if (!jsonParameter["groupDefaultValues"].isNull()) {
            QMap<QString, double> defaultGroupValues;
            
            if (jsonParameter["groupDefaultValues"].isArray()) {
                QJsonArray defaultValuesArray = jsonParameter["groupDefaultValues"].toArray();
                
                for (int i = 0; i < groups.size(); i++) {
                    defaultGroupValues.insert(groups.at(i), defaultValuesArray.at(i).toDouble());
                }
            } else {
                for (int i = 0; i < groups.size(); i++) {
                    defaultGroupValues.insert(groups.at(i), jsonParameter["groupDefaultValues"].toDouble());
                }
            }
            
            parameter->setDefaultGroupValues(defaultGroupValues);
            
            if (overrideGroupDefaultValues) {
                QMap<QString, QList<double> > groupValues;
                
                for (QMap<QString, double>::const_iterator it = defaultGroupValues.begin(); it != defaultGroupValues.end(); it++) {
                    WaterQualityParameter *groupParameter = configuration->getParameter(it.key(), WaterQualityParameterSection::PARAMETER);
                    int groupCount = groupParameter->getValue();
                    
                    if (groupCount > 0) {
                        groupValues.insert(it.key(), QVector<double>(groupCount, it.value()).toList());
                    }
                }
                
                parameter->setGroupValues(groupValues);
            }
        }
        
        WaterQualityParameter *parentParameter = configuration->getParameter(jsonParameter["parentName"].toString(), WaterQualityParameterSection::INITIAL_CONDITION);
        
        parameter->setParent(parentParameter);
        configuration->addWaterQualityParameter(parameter);
    }
    
    configuration->setLoaded(true);
}

FoodMatrixElement* WaterQualityRepository::findPreyByName(const QString &name) {
	for (FoodMatrixElement *prey : preys) {
        if (prey->getName() == name) {
            return prey;
        }
    }

    return nullptr;
}

QStringList WaterQualityRepository::findGroups(WaterQualityConfiguration *configuration, const QString &parentGroupName) const {
    WaterQualityParameter *parentGroupItem = nullptr;
    QStringList groups;
    
    for (WaterQualityParameter *parameter : configuration->getParameters()) {
        if (parameter->getName() == parentGroupName && parameter->getSection() == WaterQualityParameterSection::PARAMETER) {
            parentGroupItem = parameter;
            break;
        }
    }
    
    if (parentGroupItem) {
        for (WaterQualityParameter *child : parentGroupItem->getChildren()) {
            groups.append(child->getName());
        }
    }
    
    return groups;
}

QList<FoodMatrixElement*> WaterQualityRepository::getPredators() const {
    return predators;
}

QList<FoodMatrixElement*> WaterQualityRepository::getPreys() const {
    return preys;
}

double WaterQualityRepository::getFoodMatrixValue(FoodMatrixElement *predator, FoodMatrixElement *prey) const {
    for (FoodMatrixItem fm : defaultFoodMatrix) {
        if (fm.getPredator() == predator && fm.getPrey() == prey) {
            return fm.getValue();
        }
    }
    
    return std::numeric_limits<double>::max();
}