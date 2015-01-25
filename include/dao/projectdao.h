#ifndef PROJECTDAO_H
#define PROJECTDAO_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

#include "include/domain/project.h"
#include "include/utility/databaseutility.h"

class ProjectDAO {
private:
    QString database_name;
public:
    ProjectDAO(QString &database_name);
    ~ProjectDAO();

    void create(Project *project);
};

#endif // PROJECTDAO_H
