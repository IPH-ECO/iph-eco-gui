#include "include/services/projectservice.h"

void ProjectService::setApplicationProject(QString &name, QString &description, QString &analysis) {
    Project *project = new Project(name, description, analysis);
    IPHApplication::setCurrentProject(project);
}

void ProjectService::open(QString &filename) {
    try {
        ProjectDAO projectDAO(filename);
        Project *project = projectDAO.open();

        IPHApplication::setCurrentProject(project);
    } catch (DatabaseException &ex) {
        throw ex;
    }
}

void ProjectService::create(QString &filename) {
    try {
        Project *project = IPHApplication::getCurrentProject();
        ProjectDAO projectDAO(filename);

        projectDAO.create(project);
    } catch (DatabaseException &ex) {
        throw ex;
    }
}
