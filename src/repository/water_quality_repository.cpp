#include <repository/water_quality_repository.h>

#include <domain/water_quality_parameter.h>

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

WaterQualityRepository* WaterQualityRepository::instance = nullptr;

WaterQualityRepository* WaterQualityRepository::getInstance() {
    if (instance == nullptr) {
        instance = new WaterQualityRepository();
    }
    
    return instance;
}

WaterQualityRepository::WaterQualityRepository() {
    QFile dataFile(":/data/water_quality_parameters.json");
    
    dataFile.open(QFile::ReadOnly);
    
    QJsonDocument jsonDocument = QJsonDocument::fromJson(dataFile.readAll());
    QJsonArray jsonParameters = jsonDocument.array();
    
    dataFile.close();
    
    for (int i = 0; i < jsonParameters.size(); i++) {
        QJsonObject jsonParameter = jsonParameters[i].toObject();
        WaterQualityParameter *parameter = new WaterQualityParameter();
        
        parameter->setName(jsonParameter["name"].toString());
        parameter->setLabel(jsonParameter["label"].toString());
        parameter->setCheckable(jsonParameter["checkable"].toBool());
        parameter->setChecked(jsonParameter["checked"].toBool());
        parameter->setEnabled(jsonParameter["enabled"].toBool(true));
        parameter->setInputType(WaterQualityParameter::mapInputTypeFromString(jsonParameter["inputType"].toString()));
        parameter->setRangeMinimum(jsonParameter["rangeMinimum"].toDouble());
        parameter->setRangeMaximum(jsonParameter["rangeMaximum"].toDouble());
        parameter->setValue(jsonParameter["defaultValue"].toDouble());
        
        QString parentName = jsonParameter["parentName"].toString();
        
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

QList<WaterQualityParameter*> WaterQualityRepository::getParameters() const {
    return parameters;
}

QList<WaterQualityParameter*> WaterQualityRepository::getRootParameters() const {
    return rootParameters;
}