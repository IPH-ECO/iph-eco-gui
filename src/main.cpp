#include <application/iph_application.h>
#include <ui/main_window.h>

#include <clocale>

int main(int argc, char *argv[]) {
	std::setlocale(LC_ALL, "C");

    IPHApplication a(argc, argv);

    QApplication::setOrganizationDomain("4techlabs.com.br");
    QApplication::setOrganizationName("4techlabs");
    QApplication::setApplicationName("IPH-ECO");
    QApplication::setApplicationVersion("1.0.0");

    MainWindow w;
    w.showMaximized();

    return a.exec();
}
