#ifndef IPH_APPLICATION
#define IPH_APPLICATION

#include <QApplication>

#include <domain/project.h>
#include <ui/main_window.h>

class IPHApplication : public QApplication {
private:
    static Project *currentProject;
    static MainWindow *mainWindow;

public:
    IPHApplication(int argc, char *argv[]);
    ~IPHApplication();

    static void setCurrentProject(Project *project);
    static Project* getCurrentProject();
    static void setMainWindow(MainWindow *window);
    static MainWindow* getMainWindow();
    static int getApplicationId();

};

#endif // IPH_APPLICATION
