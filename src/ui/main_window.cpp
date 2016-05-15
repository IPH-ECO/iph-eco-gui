#include <ui/main_window.h>
#include "ui_main_window.h"

#include <application/iph_application.h>
#include <domain/simulation.h>
#include <exceptions/database_exception.h>
#include <repository/project_repository.h>
#include <ui/structured_mesh_dialog.h>
#include <ui/grid_data_dialog.h>
#include <ui/hydrodynamic_data_dialog.h>
#include <ui/create_simulation_dialog.h>
#include <ui/simulation_manager_dialog.h>
#include <ui/view_results_dialog.h>
#include <ui/water_quality_dialog.h>
#include <ui/meteorological_data_dialog.h>
#include <ui/about_dialog.h>

#include <QIcon>
#include <QMessageBox>
#include <QProgressDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mdiArea(new QMdiArea()),
    currentSubWindow(new QMdiSubWindow(mdiArea)),
    MAX_RECENT_FILES(4),
    RECENT_FILES_KEY("recent_files"),
    PROJECT_DEFAULT_DIR_KEY("default_dir")
{
    ui->setupUi(this);
    currentSubWindow->setWindowFlags(currentSubWindow->windowFlags() | Qt::FramelessWindowHint);
    currentSubWindow->setWindowState(currentSubWindow->windowState() | Qt::WindowMaximized);
    setCentralWidget(mdiArea);
    
    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);
    readSettings();
    updateRecentFilesActionList();
}

MainWindow::~MainWindow() {
    delete appSettings;
    delete ui;
}

void MainWindow::on_actionOpenProject_triggered() {
    on_actionCloseProject_triggered();

    QString filename = QFileDialog::getOpenFileName(this, tr("Select a project file"), getDefaultDirectory(), tr("IPH-ECO Project File (*.iph)"));

    openProject(filename);
}

void MainWindow::on_actionSaveProject_triggered() {
    Project *project = IPHApplication::getCurrentProject();

    if (!project->isPersisted()) {
        QString absoluteFileName = getDefaultDirectory() + "/" + project->getName();
        QString projectFileName = QFileDialog::getSaveFileName(this, tr("Save IPH-ECO Project"), absoluteFileName, tr("IPH-ECO Project file (*.iph)"));
        
        if (projectFileName.isEmpty()) {
            return;
        }
        
        project->setFilename(projectFileName);
    }

    ProjectRepository projectRepository(project->getFilename());
    QProgressDialog *progressDialog = new QProgressDialog(this);
    int maximum = projectRepository.getMaximumSaveProgress();
    
    progressDialog->setMinimum(0);
    progressDialog->setMaximum(maximum);
    progressDialog->setMinimumDuration(500);
    progressDialog->setWindowModality(Qt::WindowModal);
    
    QObject::connect(&projectRepository, SIGNAL(updateProgressText(const QString&)), progressDialog, SLOT(setLabelText(const QString&)));
    QObject::connect(&projectRepository, SIGNAL(updateProgress(int)), progressDialog, SLOT(setValue(int)));
    QObject::connect(progressDialog, SIGNAL(canceled()), &projectRepository, SLOT(cancelOperation()));
    
    try {
        projectRepository.save();
    } catch (DatabaseException &ex) {
        QMessageBox::critical(this, "Save Project", ex.what());
    }
    
    if (!progressDialog->wasCanceled()) {
        appSettings->setValue(PROJECT_DEFAULT_DIR_KEY, QFileInfo(project->getFilename()).absoluteDir().absolutePath());
        updateRecentFilesList(project->getFilename());
        setWindowTitle("IPH-ECO - " + project->getName());
    }
    
    delete progressDialog;
}

void MainWindow::on_actionSaveAsProject_triggered() {
    Project *project = IPHApplication::getCurrentProject();
    
    if (project->isPersisted()) {
        QString projectFileName = QFileDialog::getSaveFileName(this, tr("Save project as..."), project->getFilename(), tr("IPH-ECO Project File (*.iph)"));

        if (!projectFileName.isEmpty()) {
            ProjectRepository projectRepository(projectFileName);
            QProgressDialog *progressDialog = new QProgressDialog(this);
            int maximum = projectRepository.getMaximumSaveProgress();

            progressDialog->setMinimum(0);
            progressDialog->setMaximum(maximum);
            progressDialog->setMinimumDuration(500);
            progressDialog->setWindowModality(Qt::WindowModal);
            
            QObject::connect(&projectRepository, SIGNAL(updateProgressText(const QString&)), progressDialog, SLOT(setLabelText(const QString&)));
            QObject::connect(&projectRepository, SIGNAL(updateProgress(int)), progressDialog, SLOT(setValue(int)));
            QObject::connect(progressDialog, SIGNAL(canceled()), &projectRepository, SLOT(cancelOperation()));
            
            try {
                projectRepository.save(true);
            } catch (DatabaseException &ex) {
                progressDialog->cancel();
                QMessageBox::critical(this, "Save As Project", ex.what());
            }
            
            if (progressDialog->wasCanceled()) {
                QFile::remove(projectFileName);
            } else {
                project->setFilename(projectFileName);
                appSettings->setValue(PROJECT_DEFAULT_DIR_KEY, QFileInfo(projectFileName).absolutePath());
                updateRecentFilesList(project->getFilename());
                setWindowTitle("IPH-ECO - " + project->getName());
            }
            
            delete progressDialog;
        }
    } else {
        this->on_actionSaveProject_triggered();
    }
}

void MainWindow::on_actionNewProject_triggered() {
    NewProjectDialog newProjectDialog(this);
    int exitCode = newProjectDialog.exec();
    
    if (exitCode == QDialog::Accepted) {
        this->closeCurrentSubWindow();
    }
}

void MainWindow::on_actionProjectProperties_triggered() {
    ProjectPropertiesDialog projectPropertiesDialog(this);
    projectPropertiesDialog.exec();
}

void MainWindow::on_actionCloseProject_triggered() {
    Project *project = IPHApplication::getCurrentProject();

    if (project) {
        QString question = "Do you want to save the project before closing the it?";
        QMessageBox::StandardButton button = QMessageBox::question(this, "IPH-ECO", question, QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
        
        if (button == QMessageBox::Yes) {
            on_actionSaveProject_triggered();
        } else if (button == QMessageBox::Cancel) {
            return;
        }
    }
    
    IPHApplication::setCurrentProject(nullptr);
    this->closeCurrentSubWindow();
    enableMenus(false);
    setWindowTitle("IPH-ECO");
}

void MainWindow::on_actionStructuredMeshGeneration_triggered() {
    this->closeCurrentSubWindow();
    StructuredMeshDialog *structuredMeshDialog = new StructuredMeshDialog(this);
    currentSubWindow->setWidget(structuredMeshDialog);
    structuredMeshDialog->show();
#ifdef __APPLE__
    this->update();
#endif
}

void MainWindow::on_actionUnstructuredMeshGeneration_triggered() {
    this->closeCurrentSubWindow();
    UnstructuredMeshDialog *unstructuredMeshDialog = new UnstructuredMeshDialog(this);
    currentSubWindow->setWidget(unstructuredMeshDialog);
    unstructuredMeshDialog->show();
#ifdef __APPLE__
    this->update();
#endif
}

void MainWindow::on_actionGridData_triggered() {
    this->closeCurrentSubWindow();
    GridDataDialog *gridDataDialog = new GridDataDialog(this);
    currentSubWindow->setWidget(gridDataDialog);
    gridDataDialog->show();
#ifdef __APPLE__
    this->update();
#endif
}

void MainWindow::on_actionHydrodynamicData_triggered() {
    this->closeCurrentSubWindow();
    HydrodynamicDataDialog *hydrodynamicDataDialog = new HydrodynamicDataDialog(this);
    currentSubWindow->setWidget(hydrodynamicDataDialog);
    hydrodynamicDataDialog->show();
#ifdef __APPLE__
    this->update();
#endif
}

void MainWindow::on_actionWaterQuality_triggered() {
    this->closeCurrentSubWindow();
    WaterQualityDialog *waterQualityDialog = new WaterQualityDialog(this);
    currentSubWindow->setWidget(waterQualityDialog);
    waterQualityDialog->show();
#ifdef __APPLE__
    this->update();
#endif
}

void MainWindow::on_actionMeteorologyData_triggered() {
    this->closeCurrentSubWindow();
    MeteorologicalDataDialog *meteorologicalDataDialog = new MeteorologicalDataDialog(this);
    currentSubWindow->setWidget(meteorologicalDataDialog);
    meteorologicalDataDialog->show();
#ifdef __APPLE__
    this->update();
#endif
}

void MainWindow::on_actionCreateSimulation_triggered() {
    CreateSimulationDialog *createSimulationDialog = new CreateSimulationDialog(this);
    createSimulationDialog->exec();
}

void MainWindow::on_actionManageSimulation_triggered() {
    this->closeCurrentSubWindow();
    SimulationManagerDialog *simulationManagerDialog = new SimulationManagerDialog(this);
    currentSubWindow->setWidget(simulationManagerDialog);
    simulationManagerDialog->show();
#ifdef __APPLE__
    this->update();
#endif
}

void MainWindow::on_actionViewResults_triggered() {
    this->closeCurrentSubWindow();
    ViewResultsDialog *viewResultsDialog = new ViewResultsDialog(this);
    currentSubWindow->setWidget(viewResultsDialog);
    viewResultsDialog->show();
#ifdef __APPLE__
    this->update();
#endif
}

void MainWindow::on_actionAbout_triggered() {
	AboutDialog *dialog = new AboutDialog(this);
	dialog->exec();
}

void MainWindow::openRecent() {
    QAction *action = qobject_cast<QAction*>(sender());
    if (action) {
        on_actionCloseProject_triggered();
        openProject(action->data().toString());
    }
}

void MainWindow::enableMenus(bool enable) {
    ui->menuPreprocessing->setEnabled(enable);
    ui->menuInput->setEnabled(enable);
    ui->menuSimulations->setEnabled(enable);
    ui->actionNewProject->setEnabled(!enable);
    ui->actionSaveProject->setEnabled(enable);
    ui->actionSaveAsProject->setEnabled(enable);
    ui->actionProjectProperties->setEnabled(enable);
    ui->actionCloseProject->setEnabled(enable);
}

void MainWindow::readSettings() {
    appSettings->beginGroup("MainWindow");
    resize(appSettings->value("size", QSize(400, 400)).toSize());
    move(appSettings->value("pos", QPoint(200, 200)).toPoint());
    appSettings->endGroup();
}

void MainWindow::writeSettings() {
    appSettings->beginGroup("MainWindow");
    appSettings->setValue("size", size());
    appSettings->setValue("pos", pos());
    appSettings->endGroup();
}

QString MainWindow::getDefaultDirectory() {
    return appSettings->value(PROJECT_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(PROJECT_DEFAULT_DIR_KEY).toString();
}

void MainWindow::openProject(const QString &filename) {
    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);

    if (file.exists()) {
        QProgressDialog *progressDialog = new QProgressDialog(this);
        
        progressDialog->setMinimum(0);
        progressDialog->setMinimumDuration(500);
        progressDialog->setWindowModality(Qt::WindowModal);
        
        try {
            ProjectRepository projectRepository(filename);
            int maximum = projectRepository.getMaximumLoadProgress();
            
            progressDialog->setMaximum(maximum);
            
            QObject::connect(&projectRepository, SIGNAL(updateProgressText(const QString&)), progressDialog, SLOT(setLabelText(const QString&)));
            QObject::connect(&projectRepository, SIGNAL(updateProgress(int)), progressDialog, SLOT(setValue(int)));
            QObject::connect(progressDialog, SIGNAL(canceled()), &projectRepository, SLOT(cancelOperation()));

            projectRepository.open();
            
            if (progressDialog->wasCanceled()) {
                IPHApplication::setCurrentProject(nullptr);
                enableMenus(false);
                setWindowTitle("IPH-ECO");
            } else {
                this->closeCurrentSubWindow();
                enableMenus(true);
                updateRecentFilesList(filename);
                setWindowTitle("IPH-ECO - " + IPHApplication::getCurrentProject()->getName());
                appSettings->setValue(PROJECT_DEFAULT_DIR_KEY, QFileInfo(filename).absoluteDir().absolutePath());
            }
        } catch (DatabaseException &ex) {
            QMessageBox::critical(this, "Open Project", ex.what());
        }
        
        delete progressDialog;
    } else {
        QMessageBox::critical(this, "Open Project", "Project file not found.");
    }
}

void MainWindow::closeEvent(QCloseEvent *closeEvent) {
    Project *project = IPHApplication::getCurrentProject();
    
    if (project) {
        QMessageBox::StandardButton button = QMessageBox::question(this, tr("IPH-ECO"), "Do you want to save the project before exiting the application?", QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
        
		if (button == QMessageBox::Yes) {
            ProjectRepository projectRepository(project->getFilename());
			projectRepository.save();
            projectRepository.close();
		} else if (button == QMessageBox::Cancel) {
			closeEvent->ignore();
			return;
		}
    }
}

void MainWindow::updateRecentFilesList(const QString &filePath) {
    QStringList recentFilePaths = appSettings->value(RECENT_FILES_KEY).toStringList();

    recentFilePaths.removeAll(filePath);
    recentFilePaths.prepend(filePath);

    while (recentFilePaths.size() > MAX_RECENT_FILES) {
        recentFilePaths.removeLast();
    }
    appSettings->setValue(RECENT_FILES_KEY, recentFilePaths);

    updateRecentFilesActionList();
}

void MainWindow::updateRecentFilesActionList() {
    QStringList recentFilePaths = appSettings->value(RECENT_FILES_KEY).toStringList();

    QMutableStringListIterator i(recentFilePaths);
    while (i.hasNext()) {
        QFile file(i.next());
        if (!file.exists()) {
            i.remove();
        }
    }

    ui->menuOpenRecent->setEnabled(!recentFilePaths.isEmpty());
    ui->menuOpenRecent->clear();

    QAction* action = 0;
    for (int i = 0; i < recentFilePaths.size(); i++) {
        action = new QAction(this);
        action->setText(recentFilePaths.at(i));
        action->setData(recentFilePaths.at(i));
        action->setVisible(true);

        QObject::connect(action, SIGNAL(triggered()), this, SLOT(openRecent()));

        ui->menuOpenRecent->addAction(action);
    }
}

QToolBar* MainWindow::getToolBar() const {
    return ui->toolBar;
}

void MainWindow::setCoordinate(double *coordinate) {
    QString xStr = QString::number(coordinate[0], 'f', 6);
    QString yStr = QString::number(coordinate[1], 'f', 6);
    
    ui->statusBar->showMessage(QString("UTM-X: %1, UTM-Y: %2").arg(xStr).arg(yStr), 5000);
}

void MainWindow::closeCurrentSubWindow() {
    QList<QAction*> actions = {
        ui->actionNewProject,
        ui->actionOpenProject,
        ui->actionSaveProject,
        ui->actionSaveAsProject,
        ui->actionProjectProperties,
        ui->actionCloseProject
    };
    
    for (QAction *action : ui->toolBar->actions()) {
        if (!actions.contains(action)) {
            ui->toolBar->removeAction(action);
        }
    }
    
    mdiArea->closeActiveSubWindow();
}