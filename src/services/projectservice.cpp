#include "include/services/projectservice.h"

void ProjectService::create(QString &filename, QString &name, QString &description, QString &analysis) {
    Project *project = NULL;
    try {
        project = new Project(name, description, analysis);

        ProjectDAO projectDAO(filename);
        projectDAO.create(project);

        IPHApplication::setCurrentProject(project);
    } catch (DatabaseException ex) {
        delete project;
        throw ex;
    }
}
