#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMdiArea>
#include <QToolBar>
#include <QSettings>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMainWindow>
#include <QMdiSubWindow>

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
    
    QToolBar* getToolBar() const;
	void setCoordinate(double *coordinate);
	void openProject(const QString &filename);

public slots:
    void on_actionSaveProject_triggered();
    void closeCurrentSubWindow();

protected:
    void closeEvent(QCloseEvent *);

private slots:
    // File
    void on_actionOpenProject_triggered();
    void on_actionSaveAsProject_triggered();
    void on_actionProjectProperties_triggered();
    void on_actionNewProject_triggered();
    void on_actionCloseProject_triggered();
	void on_actionAbout_triggered();

    // Preprocessing
    void on_actionUnstructuredMeshGeneration_triggered();
    void on_actionStructuredMeshGeneration_triggered();

    // Input
    void on_actionGridData_triggered();
    void on_actionHydrodynamicData_triggered();
    void on_actionWaterQuality_triggered();
    void on_actionMeteorologyData_triggered();
    
    // Simulations
    void on_actionCreateSimulation_triggered();
    void on_actionManageSimulation_triggered();
    void on_actionViewResults_triggered();

    void enableMenus(bool enable);
    void openRecent();

private:
    Ui::MainWindow *ui;
    QSettings *appSettings;
    QMdiArea *mdiArea;
    QMdiSubWindow *currentSubWindow;
    
    const int MAX_RECENT_FILES;
    const QString RECENT_FILES_KEY;
    const QString PROJECT_DEFAULT_DIR_KEY;

    QString getDefaultDirectory();
    void readSettings();
    void writeSettings();
    void updateRecentFilesList(const QString &_filePath);
    void updateRecentFilesActionList();
};

#endif // MAIN_WINDOW_H
