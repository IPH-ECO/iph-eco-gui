#include "include/ui/unstructured_mesh_dialog.h"
#include "ui_unstructured_mesh_dialog.h"
#include "include/ui/island_form.h"
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

	Qt::WindowFlags flags = this->windowFlags() | Qt::WindowMaximizeButtonHint;
	this->setWindowFlags(flags);

    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();

    ui->cbxMeshName->blockSignals(true);
    for (QSet<Mesh*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it) {
        if ((*it)->instanceOf("UnstructuredMesh")) {
            ui->cbxMeshName->addItem((*it)->getName());
        }
    }
    ui->cbxMeshName->setCurrentIndex(-1);
    ui->cbxMeshName->blockSignals(false);
    
    ui->lstRefinementAreas->addItem(MeshPolygon::BOUNDARY_POLYGON_NAME);
    
    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);
    connect(ui->lstIslands->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(on_islandItemEdited(const QModelIndex&, const QModelIndex&)));
}

UnstructuredMeshDialog::~UnstructuredMeshDialog() {
    delete appSettings;
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
	QString areaStr("Area: ");

	if (area == 0.0) {
		areaStr += "-";
	}
	else {
		areaStr += QString::number(area, 'f', 2) + " m\u00B2";
	}

	ui->lblDomainArea->setText(areaStr);
}

void UnstructuredMeshDialog::on_btnBoundaryFileBrowser_clicked() {
    QString extensions = "Keyhole Markup Language file (*.kml), Text file (*.txt *xyz)";
    QString boundaryFilePath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), getDefaultDirectory(), extensions);

    if (boundaryFilePath.isEmpty()) {
        return;
    }

    ui->edtBoundaryFileLine->setText(boundaryFilePath);
    appSettings->setValue(BOUNDARY_DEFAULT_DIR_KEY, QFileInfo(boundaryFilePath).absolutePath());
}

void UnstructuredMeshDialog::on_lstIslands_currentTextChanged(const QString &currentText) {
    currentIslandName = currentText;
}

void UnstructuredMeshDialog::on_islandItemEdited(const QModelIndex &topLeft, const QModelIndex &bottomRight) {
    MeshPolygon *meshPolygon = currentMesh->getMeshPolygon(currentIslandName, MeshPolygonType::ISLAND);
    meshPolygon->setName(topLeft.data().toString());
}

void UnstructuredMeshDialog::on_btnAddIsland_clicked() {
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

void UnstructuredMeshDialog::on_lstRefinementAreas_itemSelectionChanged() {
    if (ui->lstRefinementAreas->currentRow() > 0) {
        MeshPolygon *refinementPolygon = currentMesh->getMeshPolygon(ui->lstRefinementAreas->currentItem()->text(), MeshPolygonType::REFINEMENT_AREA);

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
    QString extensions = "Keyhole Markup Language file (*.kml), Text file (*.txt *xyz)";
    QString refinementFileStr = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), getDefaultDirectory(), extensions);

    if (refinementFileStr.isEmpty()) {
        return;
    }

    if (!ui->lstRefinementAreas->findItems(refinementFileStr, Qt::MatchExactly).empty()) {
        QMessageBox::information(this, tr("Unstructured Mesh Generation"), tr("Coordinates file already added."));
        return;
    }

    try {
        // TODO: Use mesh polygon name
        currentMesh->addMeshPolygon("", refinementFileStr, MeshPolygonType::REFINEMENT_AREA);

        ui->lstRefinementAreas->addItem(refinementFileStr);
        ui->lstRefinementAreas->setCurrentRow(ui->lstRefinementAreas->count() - 1);
    } catch (const MeshPolygonException &ex) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), ex.what());
    }

    appSettings->setValue(BOUNDARY_DEFAULT_DIR_KEY, QFileInfo(refinementFileStr).absolutePath());
}

void UnstructuredMeshDialog::on_btnRemoveCoordinatesFile_clicked() {
    if (ui->lstRefinementAreas->currentRow() > 0) {
        QListWidgetItem *currentItem = ui->lstRefinementAreas->currentItem();
        QString coordinatesFile = currentItem->text();

        currentMesh->removeMeshPolygon(coordinatesFile, MeshPolygonType::REFINEMENT_AREA);
        ui->lstRefinementAreas->takeItem(ui->lstRefinementAreas->currentRow());
    }
}

void UnstructuredMeshDialog::on_sbxMaximumEdgeLength_valueChanged(double value) {
    if (ui->lstRefinementAreas->currentRow() > 0) {
        MeshPolygon *refinementPolygon = currentMesh->getMeshPolygon(currentRefinementAreaName, MeshPolygonType::REFINEMENT_AREA);
        refinementPolygon->setMaximumEdgeLength(value);
    } else {
        MeshPolygon *boundaryPolygon = currentMesh->getBoundaryPolygon();

        if (boundaryPolygon != NULL) {
            boundaryPolygon->setMaximumEdgeLength(value);
        }
    }
}

void UnstructuredMeshDialog::on_sbxMinimumAngle_valueChanged(double value) {
    if (ui->lstRefinementAreas->currentRow() > 0) {
        MeshPolygon *refinementPolygon = currentMesh->getMeshPolygon(currentRefinementAreaName, MeshPolygonType::REFINEMENT_AREA);
        refinementPolygon->setMinimumAngle(value);
    } else {
        MeshPolygon *boundaryPolygon = currentMesh->getBoundaryPolygon();

        if (boundaryPolygon != NULL) {
            boundaryPolygon->setMinimumAngle(value);
        }
    }
}

void UnstructuredMeshDialog::on_btnGenerateMesh1_clicked() {
    QString boundaryFileStr = ui->edtBoundaryFileLine->text();
    
    if (!boundaryFileStr.isEmpty() || !currentMesh->isPersisted()) {
        QFile boundaryFile(boundaryFileStr);
        QFileInfo boundaryFileInfo(boundaryFile);
        
        if (!boundaryFileInfo.exists() || !boundaryFileInfo.isFile()) {
            QMessageBox::warning(this, tr("Unstructured Mesh Generation"), tr("Boundary file not found."));
            return;
        }
    }

    enableMeshForm(true);
    currentMesh->setCoordinatesDistance(ui->sbxCoordinatesDistance->value());
    
    MeshPolygon* boundaryPolygon = nullptr;
    
    CGAL::Failure_behaviour old_behaviour = CGAL::set_error_behaviour(CGAL::THROW_EXCEPTION);
    try {
        boundaryPolygon = currentMesh->addMeshPolygon(MeshPolygon::BOUNDARY_POLYGON_NAME, boundaryFileStr, MeshPolygonType::BOUNDARY);
        currentMesh->generate();
    } catch(const MeshPolygonException &e) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), e.what());
        return;
    } catch (const std::exception& e) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), tr("This triangulation does not deal with intersecting constraints."));
        return;
    }
    CGAL::set_error_behaviour(old_behaviour);
    
    ui->meshVTKWidget->render(currentMesh);
    ui->lstRefinementAreas->setCurrentRow(0);
    ui->sbxMaximumEdgeLength->setValue(boundaryPolygon->getMaximumEdgeLength());
    ui->sbxMinimumAngle->setValue(boundaryPolygon->getMinimumAngle());
}

void UnstructuredMeshDialog::on_btnGenerateMesh2_clicked() {
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
        MeshPolygon *boundaryPolygon = currentMesh->addMeshPolygon(MeshPolygon::BOUNDARY_POLYGON_NAME, boundaryFileStr, MeshPolygonType::BOUNDARY);
        
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
        ui->edtMeshName->setText(currentMesh->getName());
        ui->edtBoundaryFileLine->setText(boundaryPolygon->getFilename());
        ui->sbxCoordinatesDistance->setValue(currentMesh->getCoordinatesDistance());
        ui->lstRefinementAreas->setCurrentRow(0);
        ui->sbxMinimumAngle->setValue(currentMesh->getBoundaryPolygon()->getMinimumAngle());
        ui->sbxMaximumEdgeLength->setValue(currentMesh->getBoundaryPolygon()->getMaximumEdgeLength());
        ui->chkShowBoundaryEdges->setChecked(true);
        ui->chkShowMesh->setChecked(true);
        ui->btnCancelMesh->setText("Done");
        ui->lstIslands->clear();
        
        for (QList<MeshPolygon*>::const_iterator it = islands.begin(); it != islands.end(); it++) {
            QListWidgetItem *item = new QListWidgetItem((*it)->getName());
            
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            ui->lstIslands->addItem(item);
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
    QMessageBox::StandardButton question = QMessageBox::question(this, tr("Remove mesh"), tr("Are you sure you want to remove '") + meshName + "'? Existing grid data configurations will be also removed.");

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
    ui->chkShowBoundaryEdges->setEnabled(enable);
    ui->chkShowUtmCoordinates->setEnabled(enable);
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
    ui->lstRefinementAreas->clear();
    ui->sbxMinimumAngle->setValue(MeshPolygon::DEFAULT_MINIMUM_ANGLE);
    ui->sbxMaximumEdgeLength->setValue(MeshPolygon::DEFAULT_MAXIMUM_EDGE_LENGTH);
    ui->lblDomainArea->setText("Area: -");
    ui->lblUTMCoordinate->setText("UTM: -");
    ui->btnRemoveMesh->setEnabled(false);

    enableMeshForm(false);
}
