#include "include/repository/simulation_repository.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>
#include <QJsonDocument>
#include <QTreeWidgetItem>

void SimulationRepository::buildTree(QTreeWidget *trOutputVariables) {
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