#include "include/application/iph_application.h"

Project* IPHApplication::currentProject = NULL;

IPHApplication::IPHApplication(int argc, char *argv[]) : QApplication(argc, argv) {}

void IPHApplication::setCurrentProject(Project *project) {
    delete currentProject;
    currentProject = project;
}

Project* IPHApplication::getCurrentProject() {
    return currentProject;
}

int IPHApplication::getApplicationId() {
    return 0x19A3C0;
}

IPHApplication::~IPHApplication() {
    delete IPHApplication::getCurrentProject();
}
