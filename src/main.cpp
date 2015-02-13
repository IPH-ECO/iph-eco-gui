#include "include/application/iph_application.h"
#include "include/ui/main_window.h"

int main(int argc, char *argv[]) {
    IPHApplication a(argc, argv);

    QApplication::setOrganizationDomain("4techlabs.com.br");
    QApplication::setOrganizationName("4techlabs");
    QApplication::setApplicationName("IPH-ECO");
    QApplication::setApplicationVersion("1.0.0");

    MainWindow w;
    w.show();

    return a.exec();
}
