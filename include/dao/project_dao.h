#ifndef PROJECTDAO_H
#define PROJECTDAO_H

#include "include/domain/project.h"
#include <QSqlDatabase>

class ProjectDAO {
private:
    QString databaseName;
    
    void saveMeshes(QSqlDatabase &db, Project *project);
    void saveMeshPolygons(QSqlDatabase &db, Mesh *mesh);
public:
    ProjectDAO(const QString &_databaseName);

    Project* open();
    void save(Project *project);
};

#endif // PROJECTDAO_H
