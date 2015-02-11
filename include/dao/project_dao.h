#ifndef PROJECTDAO_H
#define PROJECTDAO_H

#include "include/domain/project.h"

class ProjectDAO {
private:
    QString databaseName;
public:
    ProjectDAO(QString &_databaseName);

    Project* open();
    void save(Project *project);
};

#endif // PROJECTDAO_H
