#include <repository/simulation_repository.h>

#include <application/iph_application.h>
#include <exceptions/database_exception.h>
#include <utility/database_utility.h>

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>
#include <QJsonDocument>
#include <QTreeWidgetItem>

void SimulationRepository::loadOutputParametersTree(QTreeWidget *trOutputVariables) {
	QFile dataFile(":/data/output_variables.json");
    
    dataFile.open(QFile::ReadOnly);
    
    QJsonDocument jsonDocument = QJsonDocument::fromJson(dataFile.readAll());
    QJsonArray jsonArray = jsonDocument.array();
    
    dataFile.close();
    
    for (int i = 0; i < jsonArray.size(); i++) {
        QJsonObject moduleJson = jsonArray[i].toObject();
    	QTreeWidgetItem *moduleItem = new QTreeWidgetItem(trOutputVariables, QStringList(moduleJson["module"].toString()));
    	QJsonArray categoriesJson = moduleJson["categories"].toArray();

    	for (int j = 0; j < categoriesJson.size(); j++) {
            QJsonObject categoryJson = categoriesJson[j].toObject();
    		QTreeWidgetItem *categoryItem = new QTreeWidgetItem(moduleItem, QStringList(categoryJson["label"].toString()));
    		QJsonArray parameteresJson = categoryJson["parameters"].toArray();

    		for (int k = 0; k < parameteresJson.size(); k++) {
                QJsonObject parameterJson = parameteresJson[k].toObject();
    			QTreeWidgetItem *parameterItem = new QTreeWidgetItem(categoryItem, QStringList(parameterJson["label"].toString()));
                parameterItem->setData(0, Qt::UserRole, QVariant(parameterJson["name"].toString()));
    			bool checked = parameterJson["selected"].toBool();

    			parameterItem->setCheckState(0, checked ? Qt::Checked : Qt::Unchecked);
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
    QSqlQuery query(database);
    bool isFinished = status == SimulationStatus::FINISHED;

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