#include <ui/structured_mesh_dialog.h>
#include "ui_structured_mesh_dialog.h"
#include <ui/main_window.h>
#include <application/iph_application.h>
#include <exceptions/mesh_polygon_exception.h>
#include <ui/island_form.h>
#include <ui/coordinate_file_dialog.h>
#include <ui/mesh_properties_dialog.h>

#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>
#include <QMdiSubWindow>
#include <QProgressDialog>

StructuredMeshDialog::StructuredMeshDialog(QWidget *parent) :
    AbstractMeshDialog(parent), ui(new Ui::StructuredMeshDialog), unsavedMesh(new StructuredMesh), currentMesh(unsavedMesh)
{
    ui->setupUi(this);
    this->vtkWidget = ui->vtkWidget;
    
    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();

    ui->cbxMeshName->blockSignals(true);
    for (Mesh *mesh : meshes) {
        if (mesh->instanceOf("StructuredMesh")) {
            ui->cbxMeshName->addItem(mesh->getName());
        }
    }
    ui->cbxMeshName->setCurrentIndex(-1);
    ui->cbxMeshName->blockSignals(false);
    
    connect(ui->lstIslands->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(on_islandItemEdited(const QModelIndex&, const QModelIndex&)));
}

StructuredMeshDialog::~StructuredMeshDialog() {
    delete unsavedMesh;
    delete ui;
}

void StructuredMeshDialog::on_cbxMeshName_currentTextChanged(const QString &meshName) {
    if (meshName.isEmpty()) {
        return;
    }
    
    currentMesh = static_cast<StructuredMesh*>(IPHApplication::getCurrentProject()->getMesh(meshName));
    MeshPolygon *boundaryPolygon = currentMesh->getBoundaryPolygon();
    QList<MeshPolygon*> islands = currentMesh->getIslands();
    
    ui->edtMeshName->setText(currentMesh->getName());
    ui->edtBoundaryFileLine->setText(boundaryPolygon->getFilename());
    ui->edtResolution->setText(QString::number(currentMesh->getResolution()));
    ui->lstIslands->clear();
    
    for (MeshPolygon *island : islands) {
        QListWidgetItem *item = new QListWidgetItem(island->getName());
        
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->lstIslands->addItem(item);
    }
    
    ui->vtkWidget->render(currentMesh);
}

void StructuredMeshDialog::on_btnNewMesh_clicked() {
    unsavedMesh->clear();
    currentMesh = unsavedMesh;
    ui->cbxMeshName->blockSignals(true);
    ui->cbxMeshName->setCurrentIndex(-1);
    ui->cbxMeshName->blockSignals(false);
    ui->edtMeshName->setText("");
    ui->edtMeshName->setFocus();
    ui->edtBoundaryFileLine->setText("");
    ui->edtResolution->setText("100.0");
    ui->lstIslands->clear();
    ui->vtkWidget->clear();
}

void StructuredMeshDialog::on_btnApplyMesh_clicked() {
    QString oldMeshName = ui->cbxMeshName->currentText();
    QString newMeshName = ui->edtMeshName->text();
    
    if (newMeshName.isEmpty()) {
        QMessageBox::warning(this, tr("Structured Mesh Generation"), tr("Mesh name can't be empty."));
        return;
    }
    
    if (!currentMesh->getBoundaryPolygon()) {
        QMessageBox::warning(this, tr("Structured Mesh Generation"), tr("Before apply changes, you must generate the mesh."));
        return;
    }
    
    if (oldMeshName.isEmpty()) { // new mesh
        Project *project = IPHApplication::getCurrentProject();
        
        if (project->containsMesh(newMeshName)) {
            QMessageBox::critical(this, tr("Structured Mesh Generation"), tr("Mesh name already used."));
            return;
        }
        
        currentMesh->setName(newMeshName);
        project->addMesh(currentMesh);
        unsavedMesh = new StructuredMesh();
        
        ui->cbxMeshName->addItem(newMeshName);
        ui->cbxMeshName->setCurrentText(newMeshName);
    } else {
        currentMesh->setName(newMeshName);
        ui->cbxMeshName->setItemText(ui->cbxMeshName->currentIndex(), newMeshName);
    }
}

void StructuredMeshDialog::on_btnRemoveMesh_clicked() {
    QString meshName = ui->cbxMeshName->currentText();
    
    if (meshName.isEmpty()) {
        return;
    }
    
    QString questionStr = tr("Are you sure you want to remove '") + meshName + "'? Any existing associations with this mesh will be also removed.";
    QMessageBox::StandardButton questionBtn = QMessageBox::question(this, tr("Remove mesh"), questionStr);
    
    if (questionBtn == QMessageBox::Yes) {
        IPHApplication::getCurrentProject()->removeMesh(currentMesh);
        ui->cbxMeshName->removeItem(ui->cbxMeshName->currentIndex());
        this->on_btnNewMesh_clicked();
    }
}

void StructuredMeshDialog::on_btnBoundaryFileBrowser_clicked() {
    CoordinateFileDialog::CoordinateFileDialogFilter filters = static_cast<CoordinateFileDialog::CoordinateFileDialogFilter>(CoordinateFileDialog::FILTER_TEXT | CoordinateFileDialog::FILTER_KML);
    CoordinateFileDialog *dialog = new CoordinateFileDialog(this, tr("Select a boundary file"), getDefaultDirectory(), filters);
    int exitCode = dialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        QString boundaryFilePath = dialog->selectedFiles().first();
        
        this->coordinateSystem = dialog->isLatitudeLongitudeChecked() ? CoordinateSystem::LATITUDE_LONGITUDE : CoordinateSystem::UTM;
        ui->edtBoundaryFileLine->setText(boundaryFilePath);
        appSettings->setValue(BOUNDARY_DEFAULT_DIR_KEY, QFileInfo(boundaryFilePath).absolutePath());
    }
}

void StructuredMeshDialog::on_btnAddIsland_clicked() {
    IslandForm *islandForm = new IslandForm(this, currentMesh);
    int exitCode = islandForm->exec();
    
    if (exitCode == QDialog::Accepted) {
        MeshPolygon *meshPolygon = islandForm->getMeshPolygon();
        QListWidgetItem *item = new QListWidgetItem(meshPolygon->getName());
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->lstIslands->addItem(item);
    }
    
    delete islandForm;
}

void StructuredMeshDialog::on_btnRemoveIsland_clicked() {
    if (!currentMeshPolygonName.isEmpty()) {
        QString questionStr = tr("Are you sure you want to remove the selected island?");
        QMessageBox::StandardButton questionBtn = QMessageBox::question(this, tr("Structured Mesh Generation"), questionStr);

        if (questionBtn == QMessageBox::Yes) {
            currentMesh->removeMeshPolygon(currentMeshPolygonName, MeshPolygonType::ISLAND);
            ui->lstIslands->takeItem(ui->lstIslands->currentRow());
        }
    }
}

void StructuredMeshDialog::on_btnGenerateMesh_clicked() {
    QString boundaryFileStr = ui->edtBoundaryFileLine->text();
    
    if (!boundaryFileStr.isEmpty() || !currentMesh->isPersisted()) {
        QFile boundaryFile(boundaryFileStr);
        QFileInfo boundaryFileInfo(boundaryFile);

        if (!boundaryFileInfo.exists() || !boundaryFileInfo.isFile()) {
            QMessageBox::warning(this, tr("Structured Mesh Generation"), tr("Boundary file not found."));
            return;
        }
    }
    
    try {
        currentMesh->addMeshPolygon(MeshPolygon::BOUNDARY_POLYGON_NAME, boundaryFileStr, MeshPolygonType::BOUNDARY, this->coordinateSystem);
    } catch (const MeshPolygonException &e) {
        QMessageBox::critical(this, tr("Structured Mesh Generation"), e.what());
        return;
    }
    
    double resolution = ui->edtResolution->text().replace(",", ".").toDouble();
    
    if (resolution == 0) {
        QMessageBox::warning(this, tr("Structured Mesh Generation"), tr("Invalid resolution value."));
        return;
    }
    
    ulong bounds[4];
    
    currentMesh->setResolution(resolution);
    currentMesh->computeBounds(bounds);
    
    int columns = (bounds[1] - bounds[0]) / currentMesh->getResolution(); // xmax - xmin
    int rows = (bounds[3] - bounds[2]) / currentMesh->getResolution(); // ymax - ymin
    QProgressDialog *progressDialog = new QProgressDialog(tr("Generating mesh..."), tr("Cancel"), 0, columns * rows - 1, this);
    QObject::connect(currentMesh, SIGNAL(updateProgress(int)), progressDialog, SLOT(setValue(int)));
    QObject::connect(progressDialog, SIGNAL(canceled()), currentMesh, SLOT(cancelGeneration()));
    progressDialog->setMinimumDuration(500);
    progressDialog->setWindowModality(Qt::WindowModal);
    
    currentMesh->generate();
    
    if (progressDialog->wasCanceled()) {
        currentMesh->cancelGeneration(false); // Set false for future computations
    } else {
        ui->vtkWidget->render(currentMesh);
    }
    
    delete progressDialog;
}

void StructuredMeshDialog::on_lstIslands_currentTextChanged(const QString &currentText) {
    currentMeshPolygonName = currentText;
}

void StructuredMeshDialog::on_islandItemEdited(const QModelIndex &topLeft, const QModelIndex &bottomRight) {
    MeshPolygon *meshPolygon = currentMesh->getMeshPolygon(currentMeshPolygonName, MeshPolygonType::ISLAND);
    meshPolygon->setName(topLeft.data().toString());
}