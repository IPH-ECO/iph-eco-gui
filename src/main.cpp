#include "include/application/iph_application.h"
#include "include/ui/main_window.h"

int main(int argc, char *argv[]) {
    IPHApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();

    return a.exec();
}
