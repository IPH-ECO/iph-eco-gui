#include "include/application/iphapplication.h"

Project* IPHApplication::currentProject = NULL;

IPHApplication::IPHApplication(int argc, char *argv[]) : QApplication(argc, argv) {}

void IPHApplication::setCurrentProject(Project *project) {
    delete currentProject;
    currentProject = project;
}

Project* IPHApplication::getCurrentProject() {
    return currentProject;
}

IPHApplication::~IPHApplication() {
    delete IPHApplication::getCurrentProject();
}
