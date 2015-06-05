#include "include/ui/unstructured_mesh_dialog.h"
#include "ui_unstructured_mesh_dialog.h"

#include <CGAL/assertions_behaviour.h>

#include "include/application/iph_application.h"
#include "include/exceptions/mesh_exception.h"
#include "include/ui/unstructured_mesh_vtk_widget.h"

UnstructuredMeshDialog::UnstructuredMeshDialog(QWidget *parent) :
    QDialog(parent),
    BOUNDARY_DEFAULT_DIR_KEY("boundary_default_dir"),
    ui(new Ui::UnstructuredMeshDialog),
    unsavedMesh(new UnstructuredMesh()),
    currentMesh(unsavedMesh)
{
    ui->setupUi(this);
//    ui->unstructuredMeshVTKWidget->setMesh(unsavedMesh);

    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);
    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();

    for (QSet<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); ++it) {
        if (dynamic_cast<UnstructuredMesh*>(*it) != NULL) {
            ui->cbxMeshName->addItem((*it)->getName());
        }
    }
    ui->cbxMeshName->setCurrentIndex(-1);
    ui->lstCoordinateFiles->addItem(MeshPolygon::BOUNDARY_POLYGON_FILENAME);
}

QString UnstructuredMeshDialog::getDefaultDirectory() {
    return appSettings->value(BOUNDARY_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(BOUNDARY_DEFAULT_DIR_KEY).toString();
}

UnstructuredMeshDialog::~UnstructuredMeshDialog() {
    delete unsavedMesh;
    delete ui;
}

void UnstructuredMeshDialog::setCoordinate(double &x, double &y) {
    QString xStr = QString::number(x, 'f', 6);
    QString yStr = QString::number(y, 'f', 6);

    ui->lblUTMCoordinate->setText(QString("Easting: %1, Northing: %2").arg(xStr).arg(yStr));
}

void UnstructuredMeshDialog::on_btnBoundaryFileBrowser_clicked() {
    QString boundaryFilePath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), getDefaultDirectory(), "Keyhole Markup Language file (*.kml)");

    if (boundaryFilePath.isEmpty()) {
        return;
    }

    ui->edtBoundaryFileLine->setText(boundaryFilePath);
    appSettings->setValue(BOUNDARY_DEFAULT_DIR_KEY, QFileInfo(boundaryFilePath).absolutePath());
}

void UnstructuredMeshDialog::on_btnAddIsland_clicked() {
    QString islandFile = QFileDialog::getOpenFileName(this, tr("Select a island file"), getDefaultDirectory(), "Keyhole Markup Language file (*.kml)");

    if (islandFile.isEmpty()) {
        return;
    }

    if (!ui->lstIslands->findItems(islandFile, Qt::MatchExactly).empty()) {
        QMessageBox::information(this, tr("Unstructured Mesh Generation"), tr("Island file already added."));
        return;
    }

    bool proceedWithAddition = true;

    if (currentMesh->isGenerated()) {
        proceedWithAddition = QMessageBox::Yes == QMessageBox::question(this, tr("Unstructured Mesh Generation"), tr("This action will clear the generated mesh. Are you sure?"));
    }

    if (!proceedWithAddition) {
        return;
    }

    try {
        if (currentMesh->isGenerated()) {
            currentMesh->clearCDT();
        }

        currentMesh->addMeshPolygon(islandFile, MeshPolygonType::ISLAND);
        ui->lstIslands->addItem(islandFile);
        ui->unstructuredMeshVTKWidget->update();
    } catch (MeshException &ex) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), ex.what());
    }
}

void UnstructuredMeshDialog::on_btnRemoveIsland_clicked() {
    QListWidgetItem *currentItem = ui->lstIslands->currentItem();

    if (currentItem != NULL) {
        bool proceedWithRemoval = true;

        if (currentMesh->isGenerated()) {
            proceedWithRemoval = QMessageBox::Yes == QMessageBox::question(this, tr("Unstructured Mesh Generation"), tr("This action will clear the generated mesh. Are you sure?"));
        }

        if (proceedWithRemoval) {
            QString islandFile = currentItem->text();
            MeshPolygon islandPolygon(islandFile, MeshPolygonType::ISLAND);

            currentMesh->removeMeshPolygon(islandPolygon);

            if (currentMesh->isGenerated()) {
                currentMesh->clearCDT();
            }

            ui->lstIslands->takeItem(ui->lstIslands->currentRow());
            ui->unstructuredMeshVTKWidget->update();
        }
    }
}

void UnstructuredMeshDialog::on_btnGenerateDomain_clicked() {
    QString boundaryFilePath = ui->edtBoundaryFileLine->text();
    double coordinatesDistance = ui->sbxCoordinatesDistance->value();

    currentMesh->setCoordinatesDistance(coordinatesDistance);

    try {
        currentMesh->buildDomain(boundaryFilePath);
        ui->unstructuredMeshVTKWidget->setMesh(currentMesh);
    } catch(MeshException &e) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), e.what());
        return;
    }

    MeshPolygon* boundaryPolygon = currentMesh->getBoundaryPolygon();

    boundaryPolygon->setOptimalParameters();

    ui->lstCoordinateFiles->setCurrentRow(0);
    ui->sbxMaximumEdgeLength->setValue(boundaryPolygon->getMaximumEdgeLength());
    ui->sbxMinimumAngle->setValue(boundaryPolygon->getMinimumAngle());
    ui->lblDomainArea->setText(QString("Area: %1 m\u00B2").arg(currentMesh->area(), 0, 'f', 3));
}

void UnstructuredMeshDialog::on_chkShowDomainBoundary_clicked() {
    currentMesh->setShowDomainBoundary(ui->chkShowDomainBoundary->isChecked());
    ui->unstructuredMeshVTKWidget->update();
}

void UnstructuredMeshDialog::on_lstCoordinateFiles_itemSelectionChanged() {
    if (ui->lstCoordinateFiles->currentRow() > 0) {
        QString filename = ui->lstCoordinateFiles->currentItem()->text();
        MeshPolygon *refinementPolygon = currentMesh->getMeshPolygon(filename, MeshPolygonType::REFINEMENT_AREA);

        ui->sbxMinimumAngle->setValue(refinementPolygon->getMinimumAngle());
        ui->sbxMaximumEdgeLength->setValue(refinementPolygon->getMaximumEdgeLength());
    } else {
        if (currentMesh->getBoundaryPolygon() != NULL) {
            ui->sbxMinimumAngle->setValue(currentMesh->getBoundaryPolygon()->getMinimumAngle());
            ui->sbxMaximumEdgeLength->setValue(currentMesh->getBoundaryPolygon()->getMaximumEdgeLength());
        }
    }
}

void UnstructuredMeshDialog::on_btnAddCoordinatesFile_clicked() {
    QString refinementFile = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), getDefaultDirectory(), "Keyhole Markup Language file (*.kml)");

    if (refinementFile.isEmpty()) {
        return;
    }

    if (!ui->lstCoordinateFiles->findItems(refinementFile, Qt::MatchExactly).empty()) {
        QMessageBox::information(this, tr("Unstructured Mesh Generation"), tr("Coordinates file already added."));
        return;
    }

    try {
        MeshPolygon *newRefinementPolygon = currentMesh->addMeshPolygon(refinementFile, MeshPolygonType::REFINEMENT_AREA);

        newRefinementPolygon->setOptimalParameters();
        ui->lstCoordinateFiles->addItem(refinementFile);
        ui->lstCoordinateFiles->setCurrentRow(ui->lstCoordinateFiles->count() - 1);
        ui->unstructuredMeshVTKWidget->update();
    } catch (MeshException &ex) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), ex.what());
    }
}

void UnstructuredMeshDialog::on_btnRemoveCoordinatesFile_clicked() {
    if (ui->lstCoordinateFiles->currentRow() > 0) {
        QListWidgetItem *currentItem = ui->lstCoordinateFiles->currentItem();
        QString coordinatesFile = currentItem->text();
        MeshPolygon refinementPolygon(coordinatesFile, MeshPolygonType::REFINEMENT_AREA);

        currentMesh->removeMeshPolygon(refinementPolygon);
        ui->lstCoordinateFiles->takeItem(ui->lstCoordinateFiles->currentRow());
        ui->unstructuredMeshVTKWidget->update();
    }
}

void UnstructuredMeshDialog::on_sbxMaximumEdgeLength_valueChanged(double value) {
    if (ui->lstCoordinateFiles->currentRow() > 0) {
        QString filename = ui->lstCoordinateFiles->currentItem()->text();
        MeshPolygon *refinementPolygon = currentMesh->getMeshPolygon(filename, MeshPolygonType::REFINEMENT_AREA);

        refinementPolygon->setMaximumEdgeLength(value);
    } else {
        MeshPolygon *boundaryPolygon = currentMesh->getBoundaryPolygon();

        if (boundaryPolygon != NULL) {
            boundaryPolygon->setMaximumEdgeLength(value);
        }
    }
}

void UnstructuredMeshDialog::on_sbxMinimumAngle_valueChanged(double value) {
    if (ui->lstCoordinateFiles->currentRow() > 0) {
        QString filename = ui->lstCoordinateFiles->currentItem()->text();
        MeshPolygon *refinementPolygon = currentMesh->getMeshPolygon(filename, MeshPolygonType::REFINEMENT_AREA);

        refinementPolygon->setMinimumAngle(value);
    } else {
        MeshPolygon *boundaryPolygon = currentMesh->getBoundaryPolygon();

        if (boundaryPolygon != NULL) {
            boundaryPolygon->setMinimumAngle(value);
        }
    }
}

void UnstructuredMeshDialog::on_btnGenerateMesh_clicked() {
    QString meshName = ui->edtMeshName->text();

    if (meshName.isEmpty()) {
        ui->tabWidget->setCurrentIndex(0);
        ui->edtMeshName->setFocus();
        QMessageBox::warning(this, tr("Unstructured Mesh Generation"), tr("Please input the mesh name."));
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

    CGAL::Failure_behaviour old_behaviour = CGAL::set_error_behaviour(CGAL::THROW_EXCEPTION);
    try {
        currentMesh->buildDomain(boundaryFileStr);
        currentMesh->generate();
        ui->unstructuredMeshVTKWidget->setMesh(currentMesh);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), tr("This triangulation does not deal with intersecting constraints."));
        CGAL::set_error_behaviour(old_behaviour);
        return;
    }
    CGAL::set_error_behaviour(old_behaviour);
}

void UnstructuredMeshDialog::on_chkShowMesh_clicked() {
    currentMesh->setShowMesh(ui->chkShowMesh->isChecked());
    ui->unstructuredMeshVTKWidget->update();
}

void UnstructuredMeshDialog::on_cbxMeshName_currentIndexChanged(int index) {
    if (index > -1) {
        enableMeshForm(true);
        ui->btnRemoveMesh->setEnabled(true);

        QString meshName = ui->cbxMeshName->currentText();
        UnstructuredMesh unstructuredMesh(meshName);
        currentMesh = static_cast<UnstructuredMesh*>(IPHApplication::getCurrentProject()->getMesh(&unstructuredMesh));
        QList<MeshPolygon*> islands = currentMesh->getIslands();

        ui->edtMeshName->setText(currentMesh->getName());
        ui->edtBoundaryFileLine->setText(currentMesh->getBoundaryPolygon()->getFilename());
        ui->sbxCoordinatesDistance->setValue(currentMesh->getCoordinatesDistance());
        ui->lstCoordinateFiles->setCurrentRow(0);
        ui->sbxMinimumAngle->setValue(currentMesh->getBoundaryPolygon()->getMinimumAngle());
        ui->sbxMaximumEdgeLength->setValue(currentMesh->getBoundaryPolygon()->getMaximumEdgeLength());
        ui->chkShowDomainBoundary->setChecked(currentMesh->getShowDomainBoundary());
        ui->chkShowMesh->setChecked(currentMesh->getShowMesh());
        ui->btnCancelMesh->setText("Done");

        ui->lstIslands->clear();
        for (QList<MeshPolygon*>::const_iterator it = islands.begin(); it != islands.end(); it++) {
            ui->lstIslands->addItem((*it)->getFilename());
        }

        currentMesh->buildDomain(currentMesh->getBoundaryPolygon()->getFilename());
        currentMesh->generate();
        ui->unstructuredMeshVTKWidget->setMesh(currentMesh);

        // ui->lblDomainArea->setText(QString("Area: %1 m\u00B2").arg(currentMesh->getBoundaryPolygon()->area(), 0, 'f', 3));
    } else {
        resetMeshForm();
    }
}

void UnstructuredMeshDialog::on_btnSaveMesh_clicked() {
    QString meshName = ui->cbxMeshName->currentIndex() == -1 ? ui->edtMeshName->text() : ui->cbxMeshName->currentText();
    // QList<MeshPolygon*> domain = currentMesh->getDomain();
    double coordinatesDistance = ui->sbxCoordinatesDistance->value();

    Project *project = IPHApplication::getCurrentProject();
    UnstructuredMesh unstructuredMesh(meshName);
    currentMesh = static_cast<UnstructuredMesh*>(project->getMesh(&unstructuredMesh));

    if (currentMesh == NULL && ui->cbxMeshName->currentIndex() == -1) {
        bool showDomainBoundary = ui->chkShowDomainBoundary->isChecked();
        bool showMesh = ui->chkShowMesh->isChecked();

        currentMesh = new UnstructuredMesh(meshName);
        // currentMesh->setDomain(domain);
        currentMesh->setCoordinatesDistance(coordinatesDistance);
        currentMesh->setShowDomainBoundary(showDomainBoundary);
        currentMesh->setShowMesh(showMesh);

        //TODO: handle duplicity
        project->addMesh(currentMesh);

        ui->cbxMeshName->addItem(meshName);
        ui->cbxMeshName->setCurrentText(meshName);
    } else {
        meshName = ui->edtMeshName->text();

        if (meshName.isEmpty()) {
            QMessageBox::warning(this, tr("Unstructured Mesh Generation"), tr("Mesh name can't be empty."));
            return;
        }

        currentMesh->setName(meshName);
        // currentMesh->setDomain(domain);
        currentMesh->setCoordinatesDistance(coordinatesDistance);

        ui->cbxMeshName->setItemText(ui->cbxMeshName->currentIndex(), currentMesh->getName());
    }
}

void UnstructuredMeshDialog::on_btnCancelMesh_clicked() {
    if (ui->btnCancelMesh->text() == "Done") {
        ui->btnCancelMesh->setText("Cancel");
    }

    resetMeshForm();
    ui->cbxMeshName->setCurrentIndex(-1);
}

void UnstructuredMeshDialog::on_btnRemoveMesh_clicked() {
    QString meshName = ui->cbxMeshName->currentText();
    QString questionStr = tr("Are you sure you want to remove '") + meshName + "'?";
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("Remove mesh"), questionStr, QMessageBox::Yes|QMessageBox::No);

    if (QMessageBox::Yes == button) {
        UnstructuredMesh *removedMesh = currentMesh;
        currentMesh = unsavedMesh;
        IPHApplication::getCurrentProject()->removeMesh(removedMesh);

        ui->cbxMeshName->removeItem(ui->cbxMeshName->currentIndex());
        ui->cbxMeshName->setCurrentIndex(-1);
        ui->unstructuredMeshVTKWidget->setMesh(NULL);
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

    ui->unstructuredMeshVTKWidget->setMesh(NULL);
    ui->edtMeshName->setFocus();
    ui->edtMeshName->clear();
    ui->edtBoundaryFileLine->clear();
    ui->lstIslands->clear();
    ui->sbxCoordinatesDistance->setValue(0.0);
    ui->lstCoordinateFiles->clear();
    ui->sbxMinimumAngle->setValue(MeshPolygon::DEFAULT_MINIMUM_ANGLE);
    ui->sbxMaximumEdgeLength->setValue(MeshPolygon::DEFAULT_MAXIMUM_EDGE_LENGTH);
    ui->lblDomainArea->setText("Area: -");
    ui->lblUTMCoordinate->setText("UTM: -");
    ui->btnRemoveMesh->setEnabled(false);

    enableMeshForm(false);
}
