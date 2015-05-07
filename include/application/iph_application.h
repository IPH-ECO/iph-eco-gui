#ifndef IPH_APPLICATION
#define IPH_APPLICATION

#include <QApplication>

#include "include/domain/project.h"

class IPHApplication : public QApplication {
private:
    static Project* currentProject;

public:
    IPHApplication(int argc, char *argv[]);
    ~IPHApplication();

    static void setCurrentProject(Project *project);
    static Project* getCurrentProject();
    static int getApplicationId();

};

#endif // IPH_APPLICATION
