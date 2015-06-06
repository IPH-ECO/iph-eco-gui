#include "include/ui/structured_mesh_dialog.h"
#include "ui_structured_mesh_dialog.h"

#include <CGAL/assertions_behaviour.h>
#include <QDebug>

#include "include/application/iph_application.h"
#include "include/domain/unstructured_mesh.h"
#include "include/exceptions/mesh_exception.h"
#include "include/exceptions/mesh_polygon_exception.h"

StructuredMeshDialog::StructuredMeshDialog(QWidget *parent) :
    QDialog(parent),
    BOUNDARY_DEFAULT_DIR_KEY("boundary_default_dir"),
    ui(new Ui::StructuredMeshDialog),
    unsavedMesh(new StructuredMesh()),
    currentMesh(unsavedMesh)
{
    ui->setupUi(this);

    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);
    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();

    for (QSet<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); ++it) {
        if ((*it)->instanceOf("StructuredMesh")) {
            ui->cbxMeshName->addItem((*it)->getName());
        }
    }
    ui->cbxMeshName->setCurrentIndex(-1);
}

StructuredMeshDialog::~StructuredMeshDialog() {
    delete unsavedMesh;
    delete ui;
}

QString StructuredMeshDialog::getDefaultDirectory() {
    return appSettings->value(BOUNDARY_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(BOUNDARY_DEFAULT_DIR_KEY).toString();
}

void StructuredMeshDialog::setCoordinate(double &x, double &y) {
    QString xStr = QString::number(x, 'f', 6);
    QString yStr = QString::number(y, 'f', 6);

    ui->lblUTMCoordinate->setText(QString("Easting: %1, Northing: %2").arg(xStr).arg(yStr));
}

void StructuredMeshDialog::setArea(const double &area) {
    ui->lblDomainArea->setText(QString("Area: %1 m\u00B2").arg(area, 0, 'f', 3));
}

void StructuredMeshDialog::on_btnBoundaryFileBrowser_clicked() {
    QString boundaryFilePath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), getDefaultDirectory(), "Keyhole Markup Language file (*.kml)");

    ui->edtBoundaryFileLine->setText(boundaryFilePath);
}

void StructuredMeshDialog::on_btnAddIsland_clicked() {
    QString islandFile = QFileDialog::getOpenFileName(this, tr("Select a island file"), getDefaultDirectory(), "Keyhole Markup Language file (*.kml)");

    if (islandFile.isEmpty()) {
        return;
    }

    if (!ui->lstIslands->findItems(islandFile, Qt::MatchExactly).empty()) {
        QMessageBox::information(this, tr("Structured Mesh Generation"), tr("Island file already added."));
        return;
    }

    try {
        currentMesh->addMeshPolygon(islandFile, MeshPolygonType::ISLAND);
        ui->lstIslands->addItem(islandFile);
    } catch (MeshPolygonException &ex) {
        QMessageBox::critical(this, tr("Structured Mesh Generation"), ex.what());
    }
}

void StructuredMeshDialog::on_btnRemoveIsland_clicked() {
    QListWidgetItem *currentItem = ui->lstIslands->currentItem();

    if (currentItem != NULL) {
        QMessageBox::StandardButton question = QMessageBox::question(this, tr("Structured Mesh Generation"), tr("Are you sure you want to remove the selected island?"));

        if (question == QMessageBox::Yes) {
            QString islandFile = currentItem->text();

            currentMesh->removeMeshPolygon(islandFile, MeshPolygonType::ISLAND);
            ui->lstIslands->takeItem(ui->lstIslands->currentRow());
        }
    }
}

void StructuredMeshDialog::on_chkShowBoundaryEdges_clicked(bool checked) {
    currentMesh->setShowBoundaryEdges(checked);
    ui->structuredMeshVTKWidget->showBoundaryEdges(checked);
}

void StructuredMeshDialog::on_chkShowMesh_clicked(bool checked) {
    currentMesh->setShowMesh(checked);
    ui->structuredMeshVTKWidget->showMesh(checked);
}

void StructuredMeshDialog::on_btnGenerateMesh_clicked() {
    QString meshName = ui->edtMeshName->text();

    if (meshName.isEmpty()) {
        ui->edtMeshName->setFocus();
        QMessageBox::warning(this, tr("Structured Mesh Generation"), tr("Please input the mesh name."));
        return;
    }

    QString boundaryFileStr = ui->edtBoundaryFileLine->text();
    QFile boundaryFile(boundaryFileStr);
    QFileInfo boundaryFileInfo(boundaryFile);

    if (!boundaryFileInfo.exists() || !boundaryFileInfo.isFile()) {
        QMessageBox::warning(this, tr("Structured Mesh Generation"), tr("Boundary file not found."));
        return;
    }

    uint resolution = ui->sbxResolution->value();

    enableMeshForm(true);

    currentMesh->setName(meshName);
    currentMesh->setResolution(resolution);

    CGAL::Failure_behaviour old_behaviour = CGAL::set_error_behaviour(CGAL::THROW_EXCEPTION);
    try {
        currentMesh->addMeshPolygon(boundaryFileStr, MeshPolygonType::BOUNDARY);
        currentMesh->generate();
        ui->structuredMeshVTKWidget->render(currentMesh);
    } catch (const MeshPolygonException& e) {
        QMessageBox::critical(this, tr("Structured Mesh Generation"), e.what());
    }
    CGAL::set_error_behaviour(old_behaviour);
}

void StructuredMeshDialog::on_btnSaveMesh_clicked() {
    QString meshName = ui->cbxMeshName->currentIndex() == -1 ? ui->edtMeshName->text() : ui->cbxMeshName->currentText();
    Project *project = IPHApplication::getCurrentProject();

    currentMesh->setName(meshName);

    if (ui->cbxMeshName->currentIndex() == -1) {
        if (project->containsMesh(meshName)) {
            QMessageBox::critical(this, tr("Structured Mesh Generation"), tr("Mesh name already used."));
            return;
        }

        project->addMesh(currentMesh);
        unsavedMesh = new StructuredMesh();

        ui->cbxMeshName->addItem(meshName);
        ui->cbxMeshName->setCurrentText(meshName);
    } else {
        ui->cbxMeshName->setItemText(ui->cbxMeshName->currentIndex(), currentMesh->getName());
    }
}

void StructuredMeshDialog::on_btnCancelMesh_clicked() {
    if (ui->btnCancelMesh->text() == "Done") {
        ui->btnCancelMesh->setText("Cancel");
    }

    resetMeshForm();
    ui->cbxMeshName->setCurrentIndex(-1);
}

void StructuredMeshDialog::on_btnRemoveMesh_clicked() {
    QString meshName = ui->cbxMeshName->currentText();
    QMessageBox::StandardButton question = QMessageBox::question(this, tr("Remove mesh"), tr("Are you sure you want to remove '") + meshName + "'?");

    if (question == QMessageBox::Yes) {
        IPHApplication::getCurrentProject()->removeMesh(currentMesh);
        currentMesh = unsavedMesh;

        ui->cbxMeshName->removeItem(ui->cbxMeshName->currentIndex());
        ui->cbxMeshName->setCurrentIndex(-1);
        ui->structuredMeshVTKWidget->clear();
    }
}

void StructuredMeshDialog::enableMeshForm(bool enable) {
    ui->chkShowMesh->setEnabled(enable);
    ui->chkShowUtmCoordinates->setEnabled(enable);
    ui->btnSaveMesh->setEnabled(enable);
    ui->btnCancelMesh->setEnabled(enable);
}

void StructuredMeshDialog::resetMeshForm() {
    unsavedMesh->clear();
    currentMesh = unsavedMesh;

    ui->structuredMeshVTKWidget->clear();
    ui->edtMeshName->setFocus();
    ui->edtMeshName->clear();
    ui->edtBoundaryFileLine->clear();
    ui->sbxResolution->setValue(100);
    ui->lstIslands->clear();
    ui->lblDomainArea->setText("Area: -");
    ui->lblUTMCoordinate->setText("UTM: -");
    ui->btnRemoveMesh->setEnabled(false);

    enableMeshForm(false);
}

void StructuredMeshDialog::on_cbxMeshName_currentIndexChanged(int index) {
    if (index > -1) {
        enableMeshForm(true);
        ui->btnRemoveMesh->setEnabled(true);

        QString meshName = ui->cbxMeshName->currentText();
        currentMesh = static_cast<StructuredMesh*>(IPHApplication::getCurrentProject()->getMesh(meshName));
        MeshPolygon *boundaryPolygon = currentMesh->getBoundaryPolygon();
        QList<MeshPolygon*> islands = currentMesh->getIslands();

        ui->edtMeshName->setText(currentMesh->getName());
        ui->edtBoundaryFileLine->setText(boundaryPolygon->getFilename());
        ui->sbxResolution->setValue(currentMesh->getResolution());
        ui->btnCancelMesh->setText("Done");

        ui->lstIslands->clear();
        for (QList<MeshPolygon*>::const_iterator it = islands.begin(); it != islands.end(); it++) {
            ui->lstIslands->addItem((*it)->getFilename());
        }

        ui->structuredMeshVTKWidget->render(currentMesh);
    } else {
        resetMeshForm();
    }
}
