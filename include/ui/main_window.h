#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QFileDialog>
#include <QMainWindow>
#include <QSettings>

#include "include/services/project_service.h"

#include "new_project_dialog.h"
#include "project_properties_dialog.h"
#include "unstructured_mesh_widget.h"
#include "structured_grid_dialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void on_actionOpenProject_triggered();
    void on_actionSaveAsProject_triggered();
    void on_actionSaveProject_triggered();
    void on_actionProjectProperties_triggered();
    void on_actionNewProject_triggered();
    void on_actionImportUnstructuredGridGeneration_triggered();
    void on_actionCloseProject_triggered();
    void on_actionSobre_triggered();

    void enableMenus(bool enable);

private:
    Ui::MainWindow *ui;
    QSettings *appSettings;

    void readSettings();
    void writeSettings();
};

#endif // MAINWINDOW_H
