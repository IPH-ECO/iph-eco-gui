#include <application/iph_application.h>
#include <repository/water_quality_repository.h>

Project* IPHApplication::currentProject = nullptr;

MainWindow* IPHApplication::mainWindow = nullptr;

IPHApplication::IPHApplication(int argc, char *argv[]) : QApplication(argc, argv) {}

void IPHApplication::setCurrentProject(Project *project) {
    delete currentProject;
    currentProject = project;
}

Project* IPHApplication::getCurrentProject() {
    return currentProject;
}

void IPHApplication::setMainWindow(MainWindow *window) {
    mainWindow = window;
}

MainWindow* IPHApplication::getMainWindow() {
    return mainWindow;
}

int IPHApplication::getApplicationId() {
    return 0x19A3C0;
}

IPHApplication::~IPHApplication() {
    /*WaterQualityRepository *repository = WaterQualityRepository::getInstance();
    
    if (repository) {
        delete repository;
    }*/
    
    delete currentProject;
}
