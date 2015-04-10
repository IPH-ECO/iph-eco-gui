#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QFileDialog>
#include <QMainWindow>
#include <QMdiArea>
#include <QSettings>

#include "include/services/project_service.h"

#include "new_project_dialog.h"
#include "project_properties_dialog.h"
#include "unstructured_mesh_dialog.h"

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
    void on_actionUnstructuredMeshGeneration_triggered();
    void on_actionStructuredMeshGeneration_triggered();
    void on_actionCloseProject_triggered();
    void on_actionSobre_triggered();

    void enableMenus(bool enable);
    void openRecent();

private:
    const int MAX_RECENT_FILES;
    const QString RECENT_FILES_KEY;
    const QString DEFAULT_DIR_KEY;

    Ui::MainWindow *ui;
    QSettings *appSettings;
    QMdiArea *mdiArea;

    QString getDefaultDirectory();
    void readSettings();
    void writeSettings();
    void updateRecentFilesList(const QString &_filePath);
    void updateRecentFilesActionList();
    void openProject(const QString &filename);
};

#endif // MAINWINDOW_H
