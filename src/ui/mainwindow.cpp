#include "include/ui/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    projectService(new ProjectService)
{
    ui->setupUi(this);

    NewProjectDialog *newProjectDialog = new NewProjectDialog();
    connect(ui->actionNewProject, SIGNAL(triggered()), newProjectDialog, SLOT(show()));

    ProjectPropertiesDialog *projectPropertiesDialog = new ProjectPropertiesDialog();
    connect(ui->actionProjectProperties, SIGNAL(triggered()), projectPropertiesDialog, SLOT(show()));

    StructuredGridDialog *structuredGridDialog = new StructuredGridDialog();
    connect(ui->actionImport_Structured_File, SIGNAL(triggered()), structuredGridDialog, SLOT(show()));

    UnstructuredGridDialog *unstructuredGridDialog = new UnstructuredGridDialog();
    connect(ui->actionImport_Unstructured_File, SIGNAL(triggered()), unstructuredGridDialog, SLOT(show()));
}

void MainWindow::on_actionOpenProject_triggered() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Select a project file"), ".", tr("IPH-ECO Project File (*.iph)"));

    try {
        projectService->open(filename);
    } catch (DatabaseException ex) {
        QMessageBox::critical(this, "Open Project", ex.what());
    }
}

void MainWindow::on_actionSaveProject_triggered() {
    QString projectFileName = QFileDialog::getSaveFileName(this, tr("Save IPH-ECO Project"), QDir::homePath(), tr("IPH-ECHO Project file (*.iph)"));

    if (!projectFileName.isEmpty()) {
        try {
            projectService->create(projectFileName);
        } catch (DatabaseException ex) {
            QMessageBox::critical(this, "New Project", ex.what());
        }
    }
}

void MainWindow::on_actionSaveAsProject_triggered() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Save project as..."), ".", tr("IPH-ECO Project File (*.iph)"));

    if (!filename.isEmpty()) {
        try {
            projectService->create(filename);
        } catch (DatabaseException ex) {
            QMessageBox::critical(this, "Save As Project", ex.what());
        }
    }
}

MainWindow::~MainWindow() {
    delete ui;
    delete projectService;
}
