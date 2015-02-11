#include "include/ui/main_window.h"
#include "ui_main_window.h"

#include "include/exceptions/database_exception.h"
#include "include/application/iph_application.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionOpenProject_triggered() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Select a project file"), QDir::homePath(), tr("IPH-ECO Project File (*.iph)"));

    if (!filename.isEmpty()) {
        try {
            ProjectService projectService;
            projectService.open(filename);

            enableMenus(true);
        } catch (DatabaseException &ex) {
            QMessageBox::critical(this, "Open Project", ex.what());
        }
    }
}

void MainWindow::on_actionSaveProject_triggered() {
    Project *project = IPHApplication::getCurrentProject();

    if (project->getFilename().isEmpty()) {
        project->setFilename(QFileDialog::getSaveFileName(this, tr("Save IPH-ECO Project"), QDir::homePath(), tr("IPH-ECHO Project file (*.iph)")));
    }

    if (!project->getFilename().isEmpty()) {
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

    QString filename = QFileDialog::getSaveFileName(this, tr("Save project as..."), ".", tr("IPH-ECO Project File (*.iph)"));

    if (!filename.isEmpty()) {
        project->setFilename(filename);

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

void MainWindow::on_actionCloseProject_triggered()
{
    IPHApplication::setCurrentProject(NULL);
    enableMenus(false);
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

void MainWindow::on_actionSobre_triggered()
{

}
