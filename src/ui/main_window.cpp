#include "include/ui/main_window.h"
#include "ui_main_window.h"

#include "include/exceptions/database_exception.h"
#include "include/application/iph_application.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    MAX_RECENT_FILES(4),
    RECENT_FILES_KEY("recent_files"),
    DEFAULT_DIR_KEY("default_dir"),
    ui(new Ui::MainWindow)
{
    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);

    readSettings();

    ui->setupUi(this);

    updateRecentFilesActionList();
}

MainWindow::~MainWindow() {
    delete ui;
}

//slots
void MainWindow::on_actionOpenProject_triggered() {
    const QString filename = QFileDialog::getOpenFileName(this, tr("Select a project file"),
                getDefaultDirectory(), tr("IPH-ECO Project File (*.iph)"));

    openProject(filename);
}

void MainWindow::on_actionSaveProject_triggered() {
    Project *project = IPHApplication::getCurrentProject();

    if (project->getFilename().isEmpty()) {
        project->setFilename(QFileDialog::getSaveFileName(this, tr("Save IPH-ECO Project"), getDefaultDirectory(), tr("IPH-ECHO Project file (*.iph)")));
    }

    if (!project->getFilename().isEmpty()) {
        appSettings->setValue(DEFAULT_DIR_KEY, QFileInfo(project->getFilename()).absoluteDir().absolutePath());
        updateRecentFilesList(project->getFilename());

        try {
            ProjectService projectService;
            projectService.save(project);
        } catch (DatabaseException &ex) {
            QMessageBox::critical(this, "Save Project", ex.what());
        }
    }
}

void MainWindow::on_actionSaveAsProject_triggered() {
    Project *project = IPHApplication::getCurrentProject();

    QString oldFilename = project->getFilename().isEmpty() ? getDefaultDirectory() : project->getFilename();

    QString filename = QFileDialog::getSaveFileName(this, tr("Save project as..."), oldFilename, tr("IPH-ECO Project File (*.iph)"));

    if (!filename.isEmpty()) {
        appSettings->setValue(DEFAULT_DIR_KEY, QFileInfo(filename).absolutePath());

        project->setFilename(filename);

        updateRecentFilesList(project->getFilename());

        try {
            ProjectService projectService;
            projectService.save(project);
        } catch (DatabaseException &ex) {
            QMessageBox::critical(this, "Save As Project", ex.what());
        }
    }
}

void MainWindow::on_actionNewProject_triggered() {
    NewProjectDialog *newProjectDialog = new NewProjectDialog(this);
    newProjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    newProjectDialog->show();
}

void MainWindow::on_actionProjectProperties_triggered() {
    ProjectPropertiesDialog *projectPropertiesDialog = new ProjectPropertiesDialog(this, IPHApplication::getCurrentProject());
    projectPropertiesDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    projectPropertiesDialog->show();
}

void MainWindow::on_actionImportUnstructuredGridGeneration_triggered() {
    UnstructuredMeshWidget *unstructuredMeshWidget = new UnstructuredMeshWidget(this);
    unstructuredMeshWidget->setAttribute(Qt::WA_DeleteOnClose);
    this->setCentralWidget(unstructuredMeshWidget);
}

void MainWindow::on_actionCloseProject_triggered() {
    IPHApplication::setCurrentProject(0);
    enableMenus(false);
}

void MainWindow::on_actionSobre_triggered() {
    QMessageBox::about(this, tr("Sobre"), tr("IPH-ECO."));
}

void MainWindow::openRecent(){
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        openProject(action->data().toString());
    }
}

void MainWindow::enableMenus(bool enable) {
    ui->menuPreprocessing->setEnabled(enable);
    ui->menuInput->setEnabled(enable);
    ui->menuSimulations->setEnabled(enable);
    ui->menuGleon->setEnabled(enable);
    ui->actionNewProject->setEnabled(!enable);
    ui->actionSaveProject->setEnabled(enable);
    ui->actionSaveAsProject->setEnabled(enable);
    ui->actionProjectProperties->setEnabled(enable);
    ui->actionCloseProject->setEnabled(enable);
}

//methods
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
    return appSettings->value(DEFAULT_DIR_KEY).toString().isEmpty() ?
                QDir::homePath() : appSettings->value(DEFAULT_DIR_KEY).toString();
}

void MainWindow::openProject(const QString &filename) {
    if (!filename.isEmpty()) {
        appSettings->setValue(DEFAULT_DIR_KEY, QFileInfo(filename).absoluteDir().absolutePath());

        try {
            ProjectService projectService;
            projectService.open(filename);

            enableMenus(true);

            updateRecentFilesList(filename);
        } catch (DatabaseException &ex) {
            QMessageBox::critical(this, "Open Project", ex.what());
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *closeEvent) {
//    if (maybeSave()) { //TODO vai ser usado na ISSUE 11
        writeSettings();
        closeEvent->accept();
//    } else {
//        event->ignore();
//    }
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
