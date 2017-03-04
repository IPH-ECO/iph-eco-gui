#include <repository/simulation_repository.h>

#include <application/iph_application.h>
#include <exceptions/database_exception.h>
#include <utility/database_utility.h>
#include <repository/water_quality_repository.h>

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>
#include <QJsonDocument>
#include <QTreeWidgetItem>

void SimulationRepository::loadOutputParametersTree(QTreeWidget *trOutputVariables, WaterQualityConfiguration *waterQualityConfiguration) {
    QFile dataFile(":/data/output_variables.json");
    
    dataFile.open(QFile::ReadOnly);
    
    QJsonDocument jsonDocument = QJsonDocument::fromJson(dataFile.readAll());
    QJsonArray jsonArray = jsonDocument.array();
    
    dataFile.close();
    trOutputVariables->clear();
    
    for (int i = 0; i < jsonArray.size(); i++) {
        QJsonObject moduleJson = jsonArray[i].toObject();
        QString module = moduleJson.value("module").toString();
        
        if (module == "Water Quality" && !waterQualityConfiguration) {
            return;
        }
        
    	QTreeWidgetItem *moduleItem = new QTreeWidgetItem(trOutputVariables, QStringList(module));
        
        if (moduleJson["useInitialConditions"].toBool()) {
            WaterQualityRepository *waterQualityRepository = WaterQualityRepository::getInstance();
            
            waterQualityRepository->loadParameters(waterQualityConfiguration);
            
            for (WaterQualityParameter *parameter : waterQualityConfiguration->getRootParameters(WaterQualityParameterSection::INITIAL_CONDITION)) {
                appendChildren(parameter, moduleItem, module);
            }
        } else {
            QJsonArray categoriesJson = moduleJson["categories"].toArray();

            for (int j = 0; j < categoriesJson.size(); j++) {
                QJsonObject categoryJson = categoriesJson[j].toObject();
                QTreeWidgetItem *categoryItem = new QTreeWidgetItem(moduleItem, QStringList(categoryJson["label"].toString()));
                QJsonArray parameteresJson = categoryJson["parameters"].toArray();

                for (int k = 0; k < parameteresJson.size(); k++) {
                    QJsonObject parameterJson = parameteresJson[k].toObject();
                    QTreeWidgetItem *parameterItem = new QTreeWidgetItem(categoryItem, QStringList(parameterJson["label"].toString()));
                    parameterItem->setData(0, Qt::UserRole, parameterJson["name"].toString());
                    parameterItem->setData(0, Qt::UserRole + 1, module);
                    bool checked = parameterJson["selected"].toBool();

                    parameterItem->setCheckState(0, checked ? Qt::Checked : Qt::Unchecked);
                }
            }
        }
    }
}

void SimulationRepository::appendChildren(WaterQualityParameter *parameter, QTreeWidgetItem *parentItem, const QString &module) {
    QTreeWidgetItem *widgetItem = nullptr;
    
    if (parameter->getChildren().isEmpty()) {
        widgetItem = new QTreeWidgetItem(parentItem, QStringList(parameter->getLabel()));
        widgetItem->setData(0, Qt::UserRole, parameter->getName());
        widgetItem->setData(0, Qt::UserRole + 1, module);
        widgetItem->setToolTip(0, parameter->getDescription());
        widgetItem->setCheckState(0, Qt::Unchecked);
    } else {
        WaterQualityParameter *targetParameter = parameter->getTarget();
        
        if (targetParameter && targetParameter->isChecked()) {
            widgetItem = new QTreeWidgetItem(parentItem, QStringList(parameter->getLabel()));
            
            for (WaterQualityParameter *child : parameter->getChildren()) {
                appendChildren(child, widgetItem, module);
            }
        }
    }
}

QMap<QString, QString> SimulationRepository::loadOutputParametersLabels(const QStringList &parameters) {
    QMap<QString, QString> parametersLabels;
    QFile dataFile(":/data/output_variables.json");
    
    dataFile.open(QFile::ReadOnly);
    
    QJsonDocument jsonDocument = QJsonDocument::fromJson(dataFile.readAll());
    QJsonArray jsonArray = jsonDocument.array();
    
    dataFile.close();
    
    for (int i = 0; i < jsonArray.size(); i++) {
        QJsonObject moduleJson = jsonArray[i].toObject();
        QJsonArray categoriesJson = moduleJson["categories"].toArray();
        
        for (int j = 0; j < categoriesJson.size(); j++) {
            QJsonObject categoryJson = categoriesJson[j].toObject();
            QJsonArray parameteresJson = categoryJson["parameters"].toArray();
            
            for (int k = 0; k < parameteresJson.size(); k++) {
                QJsonObject parameterJson = parameteresJson[k].toObject();
                
                if (parameters.contains(parameterJson["name"].toString())) {
                    QString components = parameterJson["components"].toString();
                    QString name = parameterJson["name"].toString();
                    
                    if (components.isEmpty()) {
                        parametersLabels.insert(parameterJson["label"].toString(), name + "-");
                    } else {
                        for (QString component : components.split(",")) {
                            QString label = QString("%1 (%2)").arg(parameterJson["label"].toString()).arg(component);
                            QString nameAndComponent = QString("%1-%2").arg(name).arg(component);
                            parametersLabels.insert(label, nameAndComponent);
                        }
                    }
                }
            }
        }
    }
    
    return parametersLabels;
}

void SimulationRepository::updateSimulationStatus(Simulation *simulation, const SimulationStatus &status) {
    DatabaseUtility *databaseUtility = DatabaseUtility::getInstance();
    QSqlDatabase database(databaseUtility->getDatabase());
    bool isFinished = status == SimulationStatus::FINISHED;
    QSqlQuery query(database);

    query.prepare(QString("update simulation set status = :s %1 where id = :i").arg(isFinished ? ", progress = 100" : ""));
    query.bindValue(":s", (int) status);
    query.bindValue(":i", simulation->getId());
    
    if (!query.exec()) {
        throw DatabaseException("Unable to update simulation status.");
    }
    
    simulation->setStatus(status);
    
    if (isFinished) {
        simulation->deleteSimulationStruct();
    }
}

void SimulationRepository::updateSimulationProgress(Simulation *simulation, int progress) {
    DatabaseUtility *databaseUtility = DatabaseUtility::getInstance();
    QSqlDatabase database(databaseUtility->getDatabase());
    QSqlQuery query(database);
    
    query.prepare("update simulation set progress = :p where id = :i");
    query.bindValue(":p", progress);
    query.bindValue(":i", simulation->getId());
    
    if (!query.exec()) {
        throw DatabaseException("Unable to update simulation progress.");
    }
    
    simulation->setProgress(progress);
    
    if (progress == 100) {
        SimulationRepository::updateSimulationStatus(simulation, SimulationStatus::FINISHED);
    }
}

void SimulationRepository::deleteSimulation(Simulation *simulation) {
    DatabaseUtility *databaseUtility = DatabaseUtility::getInstance();
    QSqlDatabase database(databaseUtility->getDatabase());
    QSqlQuery query(database);
    
    query.prepare("delete from simulation where id = :i");
    query.bindValue(":i", simulation->getId());
    
    if (!query.exec()) {
        throw DatabaseException("Unable to remove simulation.");
    }
    
    Project *project = IPHApplication::getCurrentProject();
    project->removeSimulation(simulation->getLabel());
}

void SimulationRepository::saveRecoveryVariables(Simulation *simulation) {
    DatabaseUtility *databaseUtility = DatabaseUtility::getInstance();
    QSqlDatabase database(databaseUtility->getDatabase());
    QSqlQuery query(database);
    
    simulation->buildRecoveryVariablesJson();
    
    query.prepare("update simulation set recovery_variables = :r where id = :i");
    query.bindValue(":r", simulation->getRecoveryVariables());
    query.bindValue(":i", simulation->getId());
    
    if (!query.exec()) {
        throw DatabaseException("Unable to save recovery simulation variables.");
    }
}
