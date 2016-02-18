#include <application/iph_application.h>
#include <ui/main_window.h>

#ifdef _WIN32
int wmain(int argc, wchar_t *argv[], wchar_t *envp[]) {
	IPHApplication a(argc, (char **)argv);
#else
int main(int argc, char *argv[]) {
	IPHApplication a(argc, argv);
#endif

    QApplication::setOrganizationDomain("4techlabs.com.br");
    QApplication::setOrganizationName("4techlabs");
    QApplication::setApplicationName("IPH-ECO");
    QApplication::setApplicationVersion("1.0.0");

    MainWindow *mainWindow = new MainWindow();
    mainWindow->showMaximized();
    IPHApplication::setMainWindow(mainWindow);

	if (argc > 1) {
#ifdef _WIN32
		QString filename = QString::fromWCharArray(argv[1]);
#else
		QString filename = argv[1];
#endif
		mainWindow->openProject(filename);
	}

    return a.exec();
}
