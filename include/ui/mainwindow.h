#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "include/services/projectservice.h"

#include "newprojectdialog.h"
#include "projectpropertiesdialog.h"
#include "structuredgriddialog.h"
#include "unstructuredgriddialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpenProject_triggered();
    void on_actionSaveAsProject_triggered();
    void on_actionSaveProject_triggered();

private:
    Ui::MainWindow *ui;
    ProjectService *projectService;

};

#endif // MAINWINDOW_H
