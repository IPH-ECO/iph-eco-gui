#include "include/dao/projectdao.h"

ProjectDAO::ProjectDAO(QString &_database_name) : database_name(_database_name) {}

Project* ProjectDAO::open() {
    QSqlDatabase db = DatabaseUtility::connect(this->database_name);

    if (!DatabaseUtility::isDatabaseValid(db)) {
        throw DatabaseException("Invalid IPH-ECO Project file.");
    }

    QSqlQuery query(db);

    query.prepare("select * from project limit 1");
    query.exec();
    query.next();

    QString projectName = query.value("name").toString();
    QString projectDescription = query.value("description").toString();
    QString projectAnalysis = query.value("analysis").toString();

    Project *project = new Project(projectName, projectDescription, projectAnalysis);
    // TODO: Set remaining attributes
    // project->setSelectedGridDataId(0);

    return project;
}

void ProjectDAO::create(Project *project) {
    QSqlDatabase db = DatabaseUtility::connect(this->database_name);

    DatabaseUtility::createApplicationDatabase(db);

    QSqlQuery query(db);

    query.prepare("insert into project (name, description, analysis, version) values (:name, :description, :analysis, :version)");

    query.bindValue(":name", project->getName());
    query.bindValue(":description", project->getDescription());
    query.bindValue(":analysis", project->getAnalysis());
    query.bindValue(":version", project->getVersion());

    if (!query.exec()) {
        DatabaseUtility::disconnect(db);
        throw DatabaseException(QString("Unable to create project. Error: %1.").arg(query.lastError().text()).toStdString().c_str());
    }

    project->setId(query.lastInsertId().toLongLong());

    DatabaseUtility::disconnect(db);
}

