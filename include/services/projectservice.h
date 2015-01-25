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
    void create(QString &filename, QString &name, QString &description, QString &analysis);

};

#endif // PROJECTSERVICE_H
