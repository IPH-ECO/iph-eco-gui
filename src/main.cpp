#include "include/application/iphapplication.h"
#include "include/ui/mainwindow.h"

int main(int argc, char *argv[]) {
    IPHApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();

    return a.exec();
}
