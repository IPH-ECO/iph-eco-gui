#ifndef PROJECTSERVICE_H
#define PROJECTSERVICE_H

#include <QString>
#include <QSqlDatabase>

#include "include/application/iphapplication.h"
#include "include/dao/projectdao.h"

class ProjectService {
private:
//    ProjectDAO projectDAO; //TODO: Hold on heap?
public:
    void setApplicationProject(QString &name, QString &description, QString &analysis);
    void open(QString &filename);
    void create(QString &filename);

};

#endif // PROJECTSERVICE_H
