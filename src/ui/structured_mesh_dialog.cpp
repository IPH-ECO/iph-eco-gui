#include "include/ui/structured_mesh_dialog.h"
#include "ui_structured_mesh_dialog.h"

#include <CGAL/assertions_behaviour.h>

#include "include/application/iph_application.h"
#include "include/domain/unstructured_mesh.h"
#include "include/exceptions/mesh_exception.h"

StructuredMeshDialog::StructuredMeshDialog(QWidget *parent) :
    QDialog(parent),
    BOUNDARY_DEFAULT_DIR_KEY("boundary_default_dir"),
    ui(new Ui::StructuredMeshDialog),
    unsavedMesh(new StructuredMesh()),
    currentMesh(unsavedMesh)
{
    ui->setupUi(this);
    // ui->structuredMeshVTKWidget->setMesh(currentMesh);

    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);

    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();

    for (QSet<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); ++it) {
        if (dynamic_cast<UnstructuredMesh*>(*it) == NULL) {
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

    if (boundaryFilePath.isEmpty()) {
        return;
    }

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

    bool proceedWithAddition = true;

    if (currentMesh->isGenerated()) {
        proceedWithAddition = QMessageBox::Yes == QMessageBox::question(this, tr("Structured Mesh Generation"), tr("This action will clear the generated mesh. Are you sure?"));
    }

    if (!proceedWithAddition) {
        return;
    }

    try {
        currentMesh->addMeshPolygon(islandFile, MeshPolygonType::ISLAND);
        ui->lstIslands->addItem(islandFile);
        currentMesh->clearGrid();
    } catch (MeshException &ex) {
        QMessageBox::critical(this, tr("Structured Mesh Generation"), ex.what());
    }
}

void StructuredMeshDialog::on_btnRemoveIsland_clicked() {
    QListWidgetItem *currentItem = ui->lstIslands->currentItem();

    if (currentItem != NULL) {
        bool proceedWithRemoval = true;

        if (currentMesh->isGenerated()) {
            proceedWithRemoval = QMessageBox::Yes == QMessageBox::question(this, tr("Structured Mesh Generation"), tr("This action will clear the generated mesh. Are you sure?"));
        }

        if (proceedWithRemoval) {
            QString islandFile = currentItem->text();
            MeshPolygon islandPolygon(islandFile, MeshPolygonType::ISLAND);

            currentMesh->removeMeshPolygon(islandPolygon);

            if (currentMesh->isGenerated()) {
                currentMesh->clearGrid();
            }

            ui->lstIslands->takeItem(ui->lstIslands->currentRow());
            ui->structuredMeshVTKWidget->update();
        }
    }
}

void StructuredMeshDialog::on_chkShowDomainBoundary_clicked() {
    currentMesh->setShowDomainBoundary(ui->chkShowDomainBoundary->isChecked());
    ui->structuredMeshVTKWidget->update();
}

void StructuredMeshDialog::on_chkShowMesh_clicked() {
    currentMesh->setShowMesh(ui->chkShowMesh->isChecked());
    ui->structuredMeshVTKWidget->update();
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
        currentMesh->buildDomain(boundaryFileStr);
        currentMesh->generate();
        ui->structuredMeshVTKWidget->setMesh(currentMesh);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, tr("Structured Mesh Generation"), e.what());
        CGAL::set_error_behaviour(old_behaviour);
        return;
    }
    CGAL::set_error_behaviour(old_behaviour);
}

void StructuredMeshDialog::on_btnSaveMesh_clicked() {
    QString meshName = ui->cbxMeshName->currentIndex() == -1 ? ui->edtMeshName->text() : ui->cbxMeshName->currentText();
    QList<MeshPolygon*> domain = currentMesh->getDomain();
    uint resolution = ui->sbxResolution->value();

    Project *project = IPHApplication::getCurrentProject();
    StructuredMesh structuredMesh(meshName);
    currentMesh = static_cast<StructuredMesh*>(project->getMesh(&structuredMesh));

    if (currentMesh == NULL && ui->cbxMeshName->currentIndex() == -1) {
        bool showDomainBoundary = ui->chkShowDomainBoundary->isChecked();
        bool showMesh = ui->chkShowMesh->isChecked();

        currentMesh = new StructuredMesh(meshName);
        currentMesh->setDomain(domain);
        currentMesh->setResolution(resolution);
        currentMesh->setShowDomainBoundary(showDomainBoundary);
        currentMesh->setShowMesh(showMesh);

        project->addMesh(currentMesh);

        ui->cbxMeshName->addItem(meshName);
        ui->cbxMeshName->setCurrentText(meshName);
    } else {
        currentMesh->setName(meshName);
        currentMesh->setDomain(domain);
        currentMesh->setResolution(resolution);

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

    if (QMessageBox::Yes == QMessageBox::question(this, tr("Remove mesh"), tr("Are you sure you want to remove '") + meshName + "'?",
                                          QMessageBox::Yes|QMessageBox::No)) {

        StructuredMesh *removedMesh = currentMesh;
        currentMesh = unsavedMesh;
        IPHApplication::getCurrentProject()->removeMesh(removedMesh);

        ui->cbxMeshName->removeItem(ui->cbxMeshName->currentIndex());
        ui->cbxMeshName->setCurrentIndex(-1);
        ui->structuredMeshVTKWidget->setMesh(NULL);
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

    ui->structuredMeshVTKWidget->setMesh(NULL);
    ui->edtMeshName->setFocus();
    ui->edtMeshName->clear();
    ui->edtBoundaryFileLine->clear();
    ui->sbxResolution->setValue(ui->sbxResolution->minimum());
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
        StructuredMesh structuredMesh(meshName);
        currentMesh = static_cast<StructuredMesh*>(IPHApplication::getCurrentProject()->getMesh(&structuredMesh));
        MeshPolygon *boundaryPolygon = currentMesh->getBoundaryPolygon();
        QList<MeshPolygon*> islands = currentMesh->getIslands();

        ui->edtMeshName->setText(currentMesh->getName());
        ui->edtBoundaryFileLine->setText(boundaryPolygon->getFilename());
        ui->sbxResolution->setValue(currentMesh->getResolution());
        ui->chkShowDomainBoundary->setChecked(currentMesh->getShowDomainBoundary());
        ui->chkShowMesh->setChecked(currentMesh->getShowMesh());
        ui->btnCancelMesh->setText("Done");

        ui->lstIslands->clear();
        for (QList<MeshPolygon*>::const_iterator it = islands.begin(); it != islands.end(); it++) {
            ui->lstIslands->addItem((*it)->getFilename());
        }

        currentMesh->buildDomain(boundaryPolygon->getFilename());
        currentMesh->generate();
        ui->structuredMeshVTKWidget->setMesh(currentMesh);

        // ui->lblDomainArea->setText(QString("Area: %1 m\u00B2").arg(boundaryPolygon->area(), 0, 'f', 3));
    } else {
        resetMeshForm();
    }
}
