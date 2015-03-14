#include "include/ui/unstructured_mesh_dialog.h"
#include "ui_unstructured_mesh_dialog.h"

UnstructuredMeshDialog::UnstructuredMeshDialog(QWidget *parent) :
    QDialog(parent),
    BOUNDARY_DEFAULT_DIR_KEY("boundary_default_dir"),
    ui(new Ui::UnstructuredMeshDialog),
    unsavedMesh(new UnstructuredMesh()),
    currentMesh(unsavedMesh)
{

    ui->setupUi(this);
    ui->unstructuredMeshOpenGLWidget->setMesh(unsavedMesh);

    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);

    Project *project = IPHApplication::getCurrentProject();

    QSet<Mesh*> meshes = project->getMeshes();

    for (QSet<Mesh*>::iterator i = meshes.begin(); i != meshes.end(); ++i) {
        ui->cbxMeshName->addItem((*i)->getName());
    }
    ui->cbxMeshName->setCurrentIndex(-1);
}

QString UnstructuredMeshDialog::getDefaultDirectory() {
    return appSettings->value(BOUNDARY_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(BOUNDARY_DEFAULT_DIR_KEY).toString();
}

UnstructuredMeshDialog::~UnstructuredMeshDialog() {
    delete unsavedMesh;
    delete ui;
}

void UnstructuredMeshDialog::on_btnBoundaryFileBrowser_clicked() {
    QString boundaryFilePath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), getDefaultDirectory(), "Keyhole Markup Language file (*.kml)");

    if (boundaryFilePath.isEmpty()) {
        return;
    }

    ui->edtBoundaryFileLine->setText(boundaryFilePath);
    appSettings->setValue(BOUNDARY_DEFAULT_DIR_KEY, QFileInfo(boundaryFilePath).absolutePath());
}

void UnstructuredMeshDialog::on_btnGenerateDomain_clicked() {
    QString boundaryFilePath = ui->edtBoundaryFileLine->text();
    double coordinatesDistance = ui->sbxCoordinatesDistance->value();

    currentMesh->setBoundaryFilePath(boundaryFilePath);
    currentMesh->setCoordinatesDistance(coordinatesDistance);
    ui->unstructuredMeshOpenGLWidget->setMesh(currentMesh);

    try {
        ui->unstructuredMeshOpenGLWidget->generateDomain();
        ui->lblDomainArea->setText(QString("Area: %1 m\u00B2").arg(currentMesh->getBoundaryPolygon()->area(), 0, 'f', 3));
        ui->sbxMaximumEdgeLength->setValue(currentMesh->getMaximumEdgeLength());
        ui->sbxMaximumEdgeLength->setMaximum(currentMesh->height() > currentMesh->width() ? currentMesh->width() : currentMesh->height());
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
        ui->unstructuredMeshOpenGLWidget->setMesh(currentMesh);
        ui->unstructuredMeshOpenGLWidget->generateDomain();
        ui->unstructuredMeshOpenGLWidget->generateMesh();
    } catch(MeshException &e) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), e.what());
    }
}

void UnstructuredMeshDialog::on_btnSaveMesh_clicked() {
    QString meshName = ui->cbxMeshName->currentIndex() == -1 ? ui->edtMeshName->text() : ui->cbxMeshName->currentText();
    QString boundaryFile = ui->edtBoundaryFileLine->text();
    double minimumAngle = ui->sbxMinimumAngle->value();
    double maximumEdgeLength = ui->sbxMaximumEdgeLength->value();
    bool showDomainBoundary = ui->chkShowDomainBoundary->isChecked();
    bool showMesh = ui->chkShowMesh->isChecked();

    Project *project = IPHApplication::getCurrentProject();
    UnstructuredMesh unstructuredMesh(meshName);
    currentMesh = static_cast<UnstructuredMesh*>(project->getMesh(&unstructuredMesh));

    if (currentMesh == NULL && ui->cbxMeshName->currentIndex() == -1) {
        currentMesh = new UnstructuredMesh(meshName, boundaryFile, minimumAngle, maximumEdgeLength);
        currentMesh->setShowDomainBoundary(showDomainBoundary);
        currentMesh->setShowMesh(showMesh);
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

        UnstructuredMesh *removedMesh = currentMesh;
        currentMesh = unsavedMesh;
        IPHApplication::getCurrentProject()->removeMesh(removedMesh);

        ui->cbxMeshName->removeItem(ui->cbxMeshName->currentIndex());
        ui->cbxMeshName->setCurrentIndex(-1);
        ui->unstructuredMeshOpenGLWidget->setMesh(NULL);
    }
}

void UnstructuredMeshDialog::on_btnCancelMesh_clicked() {
    if (ui->btnCancelMesh->text() == "Done") {
        ui->btnCancelMesh->setText("Cancel");
    }

    resetMeshForm();
    ui->cbxMeshName->setCurrentIndex(-1);
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
        ui->chkShowDomainBoundary->setChecked(currentMesh->getShowDomainBoundary());
        ui->chkShowMesh->setChecked(currentMesh->getShowMesh());
        ui->btnCancelMesh->setText("Done");

        ui->unstructuredMeshOpenGLWidget->setMesh(currentMesh);
        ui->unstructuredMeshOpenGLWidget->generateDomain();
        ui->unstructuredMeshOpenGLWidget->generateMesh();

        ui->lblDomainArea->setText(QString("Area: %1 m\u00B2").arg(currentMesh->getBoundaryPolygon()->area(), 0, 'f', 3));
    } else {
        resetMeshForm();
    }
}

void UnstructuredMeshDialog::enableMeshForm(bool enable) {
    ui->chkShowMesh->setEnabled(enable);
    ui->chkShowEdges->setEnabled(enable);
    ui->chkShowTriangles->setEnabled(enable);
    ui->chkShowUtmCoordinates->setEnabled(enable);
    ui->chkShowVertexes->setEnabled(enable);

    ui->btnSaveMesh->setEnabled(enable);
    ui->btnCancelMesh->setEnabled(enable);
}

void UnstructuredMeshDialog::resetMeshForm() {
    unsavedMesh->clear();
    currentMesh = unsavedMesh;
    ui->unstructuredMeshOpenGLWidget->setMesh(NULL);

    ui->edtMeshName->setFocus();
    ui->edtMeshName->setText(unsavedMesh->getName());
    ui->edtBoundaryFileLine->setText(unsavedMesh->getBoundaryFilePath());
    ui->sbxMaximumEdgeLength->setValue(unsavedMesh->getMaximumEdgeLength());
    ui->sbxMinimumAngle->setValue(unsavedMesh->getMinimumAngle());
    ui->lblDomainArea->setText("Area: -");

    ui->btnRemoveMesh->setEnabled(false);
    enableMeshForm(false);
}

void UnstructuredMeshDialog::on_chkShowDomainBoundary_clicked() {
    currentMesh->setShowDomainBoundary(ui->chkShowDomainBoundary->isChecked());
}

void UnstructuredMeshDialog::on_chkShowMesh_clicked() {
    currentMesh->setShowMesh(ui->chkShowMesh->isChecked());
}
