#ifndef PROJECTSERVICE_H
#define PROJECTSERVICE_H

#include <QString>
#include <QSqlDatabase>

#include "include/application/iph_application.h"
#include "include/dao/project_dao.h"

class ProjectService {
private:

public:
    void setApplicationProject(QString &name, QString &description, bool &hydrodinamic, bool &sediment,
                               bool &waterQuality);
    void open(QString &filename);
    void save(Project *project);
    void updateProperties(QString &projectName, QString &projectDescription, bool &hydrodynamic, bool &sediment,
                          bool &waterQuality);
};

#endif // PROJECTSERVICE_H
