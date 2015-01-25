#ifndef IPHAPPLICATION
#define IPHAPPLICATION

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

};

#endif // IPHAPPLICATION
