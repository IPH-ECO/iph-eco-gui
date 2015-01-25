#include "include/dao/projectdao.h"

ProjectDAO::ProjectDAO(QString &database_name) {
    this->database_name = database_name;
}

void ProjectDAO::create(Project *project) {
    QSqlDatabase db = DatabaseUtility::connect(this->database_name);

    DatabaseUtility::createApplicationDatabase(db);

    QSqlQuery query(db);

    query.prepare("insert into project (name, description, analysis, version) " \
                  "values (:name, :description, :analysis, :version)");

    query.bindValue(":name", project->getName());
    query.bindValue(":description", project->getDescription());
    query.bindValue(":analysis", project->getAnalysis());
    query.bindValue(":version", project->getVersion());

    if (!query.exec()) {
        DatabaseUtility::disconnect(db);
        throw DatabaseException(QString("Unable to create project. Error: %1.").arg(query.lastError().text()).toStdString().c_str());
    }

    DatabaseUtility::disconnect(db);
}

ProjectDAO::~ProjectDAO(){

}

