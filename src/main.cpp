#include <application/iph_application.h>
#include <application/version.h>
#include <ui/main_window.h>

#ifdef _WIN32
int wmain(int argc, wchar_t *argv[]) {
	IPHApplication a(argc, (char **) argv);
#else
int main(int argc, char *argv[]) {
	IPHApplication a(argc, argv);
#endif

    QApplication::setOrganizationDomain(VER_COMPANYDOMAIN_STR);
    QApplication::setOrganizationName(VER_COMPANYNAME_STR);
    QApplication::setApplicationName(VER_PRODUCTNAME_STR);
    QApplication::setApplicationVersion(VER_FILEVERSION_STR);

    MainWindow *mainWindow = new MainWindow();
    mainWindow->showMaximized();
    IPHApplication::setMainWindow(mainWindow);

#ifdef _WIN32
	if (argc > 1) {
		QString filename = QString::fromWCharArray(argv[1]);
        mainWindow->openProject(filename);
	}
#endif

    return a.exec();
}
