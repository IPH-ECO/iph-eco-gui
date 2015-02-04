#include "include/ui/main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    StructuredGridDialog *structuredGridDialog = new StructuredGridDialog();
//    connect(ui->actionImport_Structured_File, SIGNAL(triggered()), structuredGridDialog, SLOT(show()));
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

            toggleMenu(true);
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

    project->setFilename(QFileDialog::getSaveFileName(this, tr("Save project as..."), ".", tr("IPH-ECO Project File (*.iph)")));

    if (!project->getFilename().isEmpty()) {
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
    ProjectPropertiesDialog *projectPropertiesDialog = new ProjectPropertiesDialog(this);
    projectPropertiesDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    projectPropertiesDialog->show();
}

void MainWindow::on_actionGenerateUnstructuredMesh_triggered() {
    UnstructuredMeshWidget *unstructuredMeshWidget = new UnstructuredMeshWidget(this);
    unstructuredMeshWidget->setAttribute(Qt::WA_DeleteOnClose);
    this->setCentralWidget(unstructuredMeshWidget);
}

void MainWindow::on_actionCloseProject_triggered()
{
    IPHApplication::setCurrentProject(NULL);
    toggleMenu(false);
}

void MainWindow::toggleMenu(bool enable) {
    ui->actionNewProject->setEnabled(!enable);
    ui->actionSaveProject->setEnabled(enable);
    ui->actionSaveAsProject->setEnabled(enable);
    ui->actionProjectProperties->setEnabled(enable);
    ui->actionCloseProject->setEnabled(enable);
}
