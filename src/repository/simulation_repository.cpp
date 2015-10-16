#include "include/repository/simulation_repository.h"

#include "include/application/iph_application.h"
#include "include/exceptions/database_exception.h"
#include "include/utility/database_utility.h"

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