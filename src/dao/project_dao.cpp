#include "include/dao/project_dao.h"

#include "include/exceptions/database_exception.h"
#include "include/utility/database_utility.h"

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

ProjectDAO::ProjectDAO(const QString &_databaseName) : databaseName(_databaseName) {}

Project* ProjectDAO::open() {
    QSqlDatabase db = DatabaseUtility::connect(this->databaseName);

    if (!DatabaseUtility::isDatabaseValid(db)) {
        throw DatabaseException("Invalid IPH-ECO Project file.");
    }

    QSqlQuery query(db);

    query.prepare("select * from project limit 1");
    query.exec();
    query.next();

    QString name = query.value("name").toString();
    QString description = query.value("description").toString();
    bool hydrodynamic = query.value("hydrodynamic").toBool();
    bool sediment = query.value("sediment").toBool();
    bool waterQuality = query.value("water_quality").toBool();

    Project *project = new Project(name, description, hydrodynamic, sediment, waterQuality);
    project->setFilename(this->databaseName);

    return project;
}

void ProjectDAO::save(Project *project) {
    QSqlDatabase db = DatabaseUtility::connect(this->databaseName);

    DatabaseUtility::createApplicationDatabase(db);

    QSqlQuery query(db);

    query.prepare("insert into project (name, description, hydrodynamic, water_quality, sediment) values "
                  "(:name, :description, :hydrodynamic, :water_quality, :sediment)");

    query.bindValue(":name", project->getName());
    query.bindValue(":description", project->getDescription());
    query.bindValue(":hydrodynamic", project->getHydrodynamic());
    query.bindValue(":water_quality", project->getWaterQuality());
    query.bindValue(":sediment", project->getSediment());

    if (!query.exec()) {
        DatabaseUtility::disconnect(db);
        throw DatabaseException(QString("Unable to create project. Error: %1.").arg(query.lastError().text()));
    }

    project->setId(query.lastInsertId().toLongLong());
    project->setDirty(false);

    DatabaseUtility::disconnect(db);
}
