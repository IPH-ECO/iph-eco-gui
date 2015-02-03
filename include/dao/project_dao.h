#ifndef PROJECTDAO_H
#define PROJECTDAO_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

#include "include/domain/project.h"
#include "include/utility/database_utility.h"

class ProjectDAO {
private:
    QString databaseName;
public:
    ProjectDAO(QString &_databaseName);

    Project* open();
    void save(Project *project);
};

#endif // PROJECTDAO_H
