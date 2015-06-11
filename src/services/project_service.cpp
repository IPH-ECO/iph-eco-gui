#include "include/services/project_service.h"

#include <QSqlDatabase>

#include "include/application/iph_application.h"
#include "include/dao/project_dao.h"

void ProjectService::setApplicationProject(QString &name, QString &description,
                                           bool &hydrodynamic, bool &sediment, bool &waterQuality) {
    Project *project = new Project(name, description, hydrodynamic, sediment, waterQuality);
    IPHApplication::setCurrentProject(project);
}

void ProjectService::open(const QString &filename) {
    ProjectDAO projectDAO(filename);
    Project *project = projectDAO.open();

    IPHApplication::setCurrentProject(project);
}

void ProjectService::save(Project *project) {
    ProjectDAO projectDAO(project->getFilename());

    projectDAO.save(project);
}

void ProjectService::updateProperties(QString &name, QString &description,
                                      bool &hydrodynamic, bool &sediment, bool &waterQuality) {
    Project *project = IPHApplication::getCurrentProject();

    project->setName(name);
    project->setDescription(description);
    project->setHydrodynamic(hydrodynamic);
    project->setSediment(sediment);
    project->setWaterQuality(waterQuality);
}
