#include "include/ui/unstructured_mesh_dialog.h"
#include "ui_unstructured_mesh_dialog.h"

UnstructuredMeshDialog::UnstructuredMeshDialog(QWidget *parent) : QDialog(parent), ui(new Ui::UnstructuredMeshDialog), unsavedMesh(new UnstructuredMesh()), currentMesh(unsavedMesh) {
    ui->setupUi(this);
}

UnstructuredMeshDialog::~UnstructuredMeshDialog() {
    delete unsavedMesh;
    delete ui;
}

void UnstructuredMeshDialog::on_btnBoundaryFileBrowser_clicked() {
    QString boundaryFilePath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), ".", "Keyhole Markup Language file (*.kml)");

    if (boundaryFilePath.isEmpty()) {
        return;
    }

    currentMesh->setBoundaryFilePath(boundaryFilePath);

    ui->edtBoundaryFileLine->setText(boundaryFilePath);

    try {
        ui->unstructuredMeshOpenGLWidget->updateCurrentMesh(currentMesh);
    } catch(MeshException &e) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), e.what());
    }
}

void UnstructuredMeshDialog::on_btnGenerateMesh_clicked() {
    QString meshName = ui->edtMeshName->text();

    if (meshName.isEmpty()) {
        QMessageBox::warning(this, tr("Unstructured Mesh Generation"), tr("Please input the mesh name."));
        ui->edtMeshName->setFocus();
        return;
    }

    QString boundaryFileStr = ui->edtBoundaryFileLine->text();
    QFile boundaryFile(boundaryFileStr);
    QFileInfo boundaryFileInfo(boundaryFile);

    if (!boundaryFileInfo.exists() || !boundaryFileInfo.isFile()) {
        QMessageBox::warning(this, tr("Unstructured Mesh Generation"), tr("Boundary file not found."));
        return;
    }

    enableMeshForm(true);

    currentMesh->setName(meshName);
    currentMesh->setBoundaryFilePath(boundaryFileStr);
    currentMesh->setMinimumAngle(ui->sbxMinimumAngle->value());
    currentMesh->setMaximumEdgeLength(ui->sbxMaximumEdgeLength->value());

    try {
        ui->unstructuredMeshOpenGLWidget->updateCurrentMesh(currentMesh);
        ui->unstructuredMeshOpenGLWidget->generateMesh();
    } catch(MeshException &e) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), e.what());
    }
}

void UnstructuredMeshDialog::on_btnResetMesh_clicked() {
    ui->sbxMinimumAngle->setValue(ui->sbxMinimumAngle->minimum());
    ui->sbxMaximumEdgeLength->setValue(ui->sbxMaximumEdgeLength->minimum());
}

void UnstructuredMeshDialog::on_btnSaveMesh_clicked() {
    QString meshName = ui->cbxMeshName->currentIndex() == -1 ? ui->edtMeshName->text() : ui->cbxMeshName->currentText();
    QString boundaryFile = ui->edtBoundaryFileLine->text();
    double minimumAngle = ui->sbxMinimumAngle->value();
    double maximumEdgeLength = ui->sbxMaximumEdgeLength->value();

    Project *project = IPHApplication::getCurrentProject();
    UnstructuredMesh unstructuredMesh(meshName);
    currentMesh = static_cast<UnstructuredMesh*>(project->getMesh(&unstructuredMesh));

    if (currentMesh == NULL && ui->cbxMeshName->currentIndex() == -1) {
        currentMesh = new UnstructuredMesh(meshName, boundaryFile, minimumAngle, maximumEdgeLength);
        project->addMesh(currentMesh);

        ui->cbxMeshName->addItem(meshName);
        ui->cbxMeshName->setCurrentText(meshName);
    } else {
        currentMesh->setName(meshName);
        currentMesh->setBoundaryFilePath(boundaryFile);
        currentMesh->setMinimumAngle(minimumAngle);
        currentMesh->setMaximumEdgeLength(maximumEdgeLength);

        ui->cbxMeshName->setItemText(ui->cbxMeshName->currentIndex(), currentMesh->getName());
    }
}

void UnstructuredMeshDialog::on_btnRemoveMesh_clicked() {
    QString meshName = ui->cbxMeshName->currentText();

    if (QMessageBox::Yes == QMessageBox::question(this, tr("Remove mesh"), tr("Are you sure you want to remove '") + meshName + "'?",
                                          QMessageBox::Yes|QMessageBox::No)) {

        UnstructuredMesh unstructuredMesh(meshName);
        IPHApplication::getCurrentProject()->removeMesh(&unstructuredMesh);

        ui->cbxMeshName->removeItem(ui->cbxMeshName->currentIndex());
        ui->cbxMeshName->setCurrentIndex(-1);
        ui->unstructuredMeshOpenGLWidget->reset();
    }
}

void UnstructuredMeshDialog::on_btnCancelMesh_clicked() {
    resetMeshForm();
    ui->cbxMeshName->setCurrentIndex(-1);
    ui->unstructuredMeshOpenGLWidget->reset();
}

void UnstructuredMeshDialog::on_cbxMeshName_currentIndexChanged(int index) {
    if (index > -1) {
        enableMeshForm(true);
        ui->btnRemoveMesh->setEnabled(true);

        QString meshName = ui->cbxMeshName->currentText();
        UnstructuredMesh unstructuredMesh(meshName);
        currentMesh = static_cast<UnstructuredMesh*>(IPHApplication::getCurrentProject()->getMesh(&unstructuredMesh));

        ui->edtMeshName->setText(currentMesh->getName());
        ui->edtBoundaryFileLine->setText(currentMesh->getBoundaryFilePath());
        ui->sbxMinimumAngle->setValue(currentMesh->getMinimumAngle());
        ui->sbxMaximumEdgeLength->setValue(currentMesh->getMaximumEdgeLength());

        ui->unstructuredMeshOpenGLWidget->updateCurrentMesh(currentMesh);
        ui->unstructuredMeshOpenGLWidget->generateMesh();
    } else {
        resetMeshForm();
    }
}

void UnstructuredMeshDialog::enableMeshForm(bool enable) {
    ui->btnRefineSpecificRegion->setEnabled(enable);
    ui->btnApplyCoarsening->setEnabled(enable);

    ui->chkShowEdges->setEnabled(enable);
    ui->chkShowTriangles->setEnabled(enable);
    ui->chkShowUtmCoordinates->setEnabled(enable);
    ui->chkShowVertexes->setEnabled(enable);

    ui->btnSaveMesh->setEnabled(enable);
    ui->btnCancelMesh->setEnabled(enable);
}

void UnstructuredMeshDialog::resetMeshForm() {
    ui->edtMeshName->setFocus();

    ui->edtMeshName->clear();
    ui->edtBoundaryFileLine->clear();
    on_btnResetMesh_clicked();

    ui->btnRemoveMesh->setEnabled(false);
    enableMeshForm(false);

    currentMesh = unsavedMesh;
}

void UnstructuredMeshDialog::on_chkShowDomainBoundary_clicked() {
    ui->unstructuredMeshOpenGLWidget->toogleDomainBoundary(ui->chkShowDomainBoundary->isChecked());
}
