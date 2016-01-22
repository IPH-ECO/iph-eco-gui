#ifndef PROJECT_SERVICE_H
#define PROJECT_SERVICE_H

#include <QString>

#include <domain/project.h>

class ProjectService {
private:

public:
    void setApplicationProject(QString &name, QString &description, bool &hydrodinamic, bool &waterQuality);
    void open(const QString &filename);
    void save(Project *project);
    void updateProperties(QString &projectName, QString &projectDescription, bool &hydrodynamic, bool &waterQuality);
};

#endif // PROJECT_SERVICE_H
