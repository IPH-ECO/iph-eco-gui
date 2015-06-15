#ifndef PROJECT_DAO_H
#define PROJECT_DAO_H

#include "include/domain/project.h"
#include "include/domain/grid_data_configuration.h"
#include <QSqlDatabase>

class ProjectDAO {
private:
    QString databaseName;
    
    void saveMeshes(QSqlDatabase &db, Project *project);
    void saveMeshPolygons(QSqlDatabase &db, Mesh *mesh);
    void loadMeshes(QSqlDatabase &db, Project *project);
    void loadMeshPolygons(QSqlDatabase &db, Mesh *mesh);
    void loadGridDataConfigurations(QSqlDatabase &db, Project *project);
    void loadGridData(QSqlDatabase &db, GridDataConfiguration *gridDataConfiguration, Project *project);
    
    void saveGridDataConfigurations(QSqlDatabase &db, Project *project);
    void saveGridData(QSqlDatabase &db, GridDataConfiguration *gridDataConfiguration);
public:
    ProjectDAO(const QString &_databaseName);

    Project* open();
    void save(Project *project);
};

#endif // PROJECT_DAO_H
