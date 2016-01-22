#include <services/project_service.h>

#include <QSqlDatabase>

#include <application/iph_application.h>
#include <repository/project_repository.h>

void ProjectService::setApplicationProject(QString &name, QString &description,
                                           bool &hydrodynamic, bool &waterQuality) {
    Project *project = new Project(name, description, hydrodynamic, waterQuality);
    IPHApplication::setCurrentProject(project);
}

void ProjectService::open(const QString &filename) {
    ProjectRepository projectRepository(filename);
    Project *project = nullptr;

    IPHApplication::setCurrentProject(project);
}

void ProjectService::save(Project *project) {
    ProjectRepository projectRepository(project->getFilename());

    projectRepository.save();
}

void ProjectService::updateProperties(QString &name, QString &description,
                                      bool &hydrodynamic, bool &waterQuality) {
    Project *project = IPHApplication::getCurrentProject();

    project->setName(name);
    project->setDescription(description);
    project->setHydrodynamic(hydrodynamic);
    project->setWaterQuality(waterQuality);
}
