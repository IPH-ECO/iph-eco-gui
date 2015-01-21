#include "../include/ui/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IPH::UI::MainWindow w;
    w.showMaximized();

    return a.exec();
}
