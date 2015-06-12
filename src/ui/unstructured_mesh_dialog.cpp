#include "include/ui/unstructured_mesh_dialog.h"
#include "ui_unstructured_mesh_dialog.h"

#include "include/application/iph_application.h"
#include "include/exceptions/mesh_polygon_exception.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <CGAL/assertions_behaviour.h>

UnstructuredMeshDialog::UnstructuredMeshDialog(QWidget *parent) :
    QDialog(parent), BOUNDARY_DEFAULT_DIR_KEY("boundary_default_dir"), ui(new Ui::UnstructuredMeshDialog),
    unsavedMesh(new UnstructuredMesh()), currentMesh(unsavedMesh)
{
    ui->setupUi(this);

    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();

    for (QSet<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); ++it) {
        if ((*it)->instanceOf("UnstructuredMesh")) {
            ui->cbxMeshName->addItem((*it)->getName());
        }
    }
    ui->cbxMeshName->setCurrentIndex(-1);
    ui->lstCoordinateFiles->addItem(MeshPolygon::BOUNDARY_POLYGON_FILENAME);
    
    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);
}

UnstructuredMeshDialog::~UnstructuredMeshDialog() {
    delete unsavedMesh;
    delete ui;
}

QString UnstructuredMeshDialog::getDefaultDirectory() {
    return appSettings->value(BOUNDARY_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(BOUNDARY_DEFAULT_DIR_KEY).toString();
}

void UnstructuredMeshDialog::setCoordinate(double &x, double &y) {
    QString xStr = QString::number(x, 'f', 6);
    QString yStr = QString::number(y, 'f', 6);

    ui->lblUTMCoordinate->setText(QString("Easting: %1, Northing: %2").arg(xStr).arg(yStr));
}

void UnstructuredMeshDialog::setArea(const double &area) {
    ui->lblDomainArea->setText(QString("Area: %1 m\u00B2").arg(area, 0, 'f', 3));
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

    try {
        currentMesh->addMeshPolygon(islandFile, MeshPolygonType::ISLAND);
        ui->lstIslands->addItem(islandFile);
    } catch (const MeshPolygonException &ex) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), ex.what());
    }
}

void UnstructuredMeshDialog::on_btnRemoveIsland_clicked() {
    QListWidgetItem *currentItem = ui->lstIslands->currentItem();

    if (currentItem != NULL) {
        QMessageBox::StandardButton question = QMessageBox::question(this, tr("Unstructured Mesh Generation"), tr("Are you sure you want to remove the selected island?"));

        if (question == QMessageBox::Yes) {
            QString islandFile = currentItem->text();

            currentMesh->removeMeshPolygon(islandFile, MeshPolygonType::ISLAND);
            ui->lstIslands->takeItem(ui->lstIslands->currentRow());
        }
    }
}

void UnstructuredMeshDialog::on_chkShowBoundaryEdges_clicked(bool checked) {
    currentMesh->setShowBoundaryEdges(checked);
    ui->meshVTKWidget->showBoundaryEdges(checked);
}

void UnstructuredMeshDialog::on_chkShowMesh_clicked(bool checked) {
    currentMesh->setShowMesh(checked);
    ui->meshVTKWidget->showMesh(checked);
}

void UnstructuredMeshDialog::on_btnGenerateDomain_clicked() {
    QString boundaryFileStr = ui->edtBoundaryFileLine->text();

    currentMesh->setCoordinatesDistance(ui->sbxCoordinatesDistance->value());

    try {
        currentMesh->addMeshPolygon(boundaryFileStr, MeshPolygonType::BOUNDARY);
        currentMesh->generate();
        ui->meshVTKWidget->render(currentMesh);
    } catch(const MeshPolygonException &e) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), e.what());
        return;
    }

    MeshPolygon* boundaryPolygon = currentMesh->getBoundaryPolygon();

    ui->lstCoordinateFiles->setCurrentRow(0);
    ui->sbxMaximumEdgeLength->setValue(boundaryPolygon->getMaximumEdgeLength());
    ui->sbxMinimumAngle->setValue(boundaryPolygon->getMinimumAngle());
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
    QString refinementFileStr = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), getDefaultDirectory(), "Keyhole Markup Language file (*.kml)");

    if (refinementFileStr.isEmpty()) {
        return;
    }

    if (!ui->lstCoordinateFiles->findItems(refinementFileStr, Qt::MatchExactly).empty()) {
        QMessageBox::information(this, tr("Unstructured Mesh Generation"), tr("Coordinates file already added."));
        return;
    }

    try {
        currentMesh->addMeshPolygon(refinementFileStr, MeshPolygonType::REFINEMENT_AREA);

        ui->lstCoordinateFiles->addItem(refinementFileStr);
        ui->lstCoordinateFiles->setCurrentRow(ui->lstCoordinateFiles->count() - 1);
    } catch (const MeshPolygonException &ex) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), ex.what());
    }

    appSettings->setValue(BOUNDARY_DEFAULT_DIR_KEY, QFileInfo(refinementFileStr).absolutePath());
}

void UnstructuredMeshDialog::on_btnRemoveCoordinatesFile_clicked() {
    if (ui->lstCoordinateFiles->currentRow() > 0) {
        QListWidgetItem *currentItem = ui->lstCoordinateFiles->currentItem();
        QString coordinatesFile = currentItem->text();

        currentMesh->removeMeshPolygon(coordinatesFile, MeshPolygonType::REFINEMENT_AREA);
        ui->lstCoordinateFiles->takeItem(ui->lstCoordinateFiles->currentRow());
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
    QString boundaryFileStr = ui->edtBoundaryFileLine->text();
    QFile boundaryFile(boundaryFileStr);
    QFileInfo boundaryFileInfo(boundaryFile);

    if (!boundaryFileInfo.exists() || !boundaryFileInfo.isFile()) {
        QMessageBox::warning(this, tr("Unstructured Mesh Generation"), tr("Boundary file not found."));
        return;
    }

    enableMeshForm(true);
    
    currentMesh->setCoordinatesDistance(ui->sbxCoordinatesDistance->value());

    CGAL::Failure_behaviour old_behaviour = CGAL::set_error_behaviour(CGAL::THROW_EXCEPTION);
    try {
        MeshPolygon *boundaryPolygon = currentMesh->addMeshPolygon(boundaryFileStr, MeshPolygonType::BOUNDARY);
        
        boundaryPolygon->setMinimumAngle(ui->sbxMinimumAngle->value());
        boundaryPolygon->setMaximumEdgeLength(ui->sbxMaximumEdgeLength->value());
        currentMesh->generate();
        ui->meshVTKWidget->render(currentMesh);
    } catch (const MeshPolygonException &e) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), e.what());
    } catch (const std::exception& e) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), tr("This triangulation does not deal with intersecting constraints."));
    }
    CGAL::set_error_behaviour(old_behaviour);
}

void UnstructuredMeshDialog::on_cbxMeshName_currentIndexChanged(int index) {
    if (index > -1) {
        QString meshName = ui->cbxMeshName->currentText();
        currentMesh = static_cast<UnstructuredMesh*>(IPHApplication::getCurrentProject()->getMesh(meshName));
        MeshPolygon *boundaryPolygon = currentMesh->getBoundaryPolygon();
        QList<MeshPolygon*> islands = currentMesh->getIslands();

        enableMeshForm(true);
        ui->btnRemoveMesh->setEnabled(true);
        ui->edtBoundaryFileLine->setText(boundaryPolygon->getFilename());
        ui->edtMeshName->setText(currentMesh->getName());
        ui->edtBoundaryFileLine->setText(boundaryPolygon->getFilename());
        ui->sbxCoordinatesDistance->setValue(currentMesh->getCoordinatesDistance());
        ui->lstCoordinateFiles->setCurrentRow(0);
        ui->sbxMinimumAngle->setValue(currentMesh->getBoundaryPolygon()->getMinimumAngle());
        ui->sbxMaximumEdgeLength->setValue(currentMesh->getBoundaryPolygon()->getMaximumEdgeLength());
        ui->chkShowBoundaryEdges->setChecked(currentMesh->getShowBoundaryEdges());
        ui->chkShowMesh->setChecked(currentMesh->getShowMesh());
        ui->btnCancelMesh->setText("Done");
        ui->lstIslands->clear();
        for (QList<MeshPolygon*>::const_iterator it = islands.begin(); it != islands.end(); it++) {
            ui->lstIslands->addItem((*it)->getFilename());
        }

        ui->meshVTKWidget->render(currentMesh);
    } else {
        resetMeshForm();
    }
}

void UnstructuredMeshDialog::on_btnSaveMesh_clicked() {
    QString meshName = ui->cbxMeshName->currentIndex() == -1 ? ui->edtMeshName->text() : ui->cbxMeshName->currentText();

    if (meshName.isEmpty()) {
        QMessageBox::warning(this, tr("Unstructured Mesh Generation"), tr("Mesh name can't be empty."));
        return;
    }

    currentMesh->setName(meshName);
    currentMesh->setCoordinatesDistance(ui->sbxCoordinatesDistance->value());

    if (ui->cbxMeshName->currentIndex() == -1) {
        Project *project = IPHApplication::getCurrentProject();

        if (project->containsMesh(meshName)) {
            QMessageBox::critical(this, tr("Unstructured Mesh Generation"), tr("Mesh name already used."));
            return;
        }

        project->addMesh(currentMesh);
        unsavedMesh = new UnstructuredMesh();

        ui->cbxMeshName->addItem(meshName);
        ui->cbxMeshName->setCurrentText(meshName);
    } else {
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
    QMessageBox::StandardButton question = QMessageBox::question(this, tr("Remove mesh"), tr("Are you sure you want to remove '") + meshName + "'?");

    if (question == QMessageBox::Yes) {
        IPHApplication::getCurrentProject()->removeMesh(currentMesh);
        currentMesh = unsavedMesh;

        ui->cbxMeshName->removeItem(ui->cbxMeshName->currentIndex());
        ui->cbxMeshName->setCurrentIndex(-1);
        ui->meshVTKWidget->clear();
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

    ui->meshVTKWidget->clear();
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
