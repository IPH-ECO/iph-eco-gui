#include <ui/unstructured_mesh_dialog.h>
#include "ui_unstructured_mesh_dialog.h"

#include <ui/island_form.h>
#include <application/iph_application.h>
#include <exceptions/mesh_polygon_exception.h>
#include <ui/coordinate_file_dialog.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QMdiSubWindow>
#include <CGAL/assertions_behaviour.h>

UnstructuredMeshDialog::UnstructuredMeshDialog(QWidget *parent) : AbstractMeshDialog(parent), ui(new Ui::UnstructuredMeshDialog),
    unsavedMesh(new UnstructuredMesh()), currentMesh(unsavedMesh)
{
    ui->setupUi(this);
    this->vtkWidget = ui->vtkWidget;

	Qt::WindowFlags flags = this->windowFlags() | Qt::WindowMaximizeButtonHint;
	this->setWindowFlags(flags);

    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();

    ui->cbxMeshName->blockSignals(true);
    for (Mesh *mesh : meshes) {
        if (mesh->instanceOf("UnstructuredMesh")) {
            ui->cbxMeshName->addItem(mesh->getName());
        }
    }
    ui->cbxMeshName->setCurrentIndex(-1);
    ui->cbxMeshName->blockSignals(false);
    
    ui->lstRefinementAreas->addItem(MeshPolygon::BOUNDARY_POLYGON_NAME);
    
    connect(ui->lstIslands->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(on_islandItemEdited(const QModelIndex&, const QModelIndex&)));
}

UnstructuredMeshDialog::~UnstructuredMeshDialog() {
    delete unsavedMesh;
    delete ui;
}

void UnstructuredMeshDialog::on_btnBoundaryFileBrowser_clicked() {
    QString extensions = "Text file (*.txt *xyz);;Keyhole Markup Language file (*.kml)";
    CoordinateFileDialog *dialog = new CoordinateFileDialog(this, tr("Select a boundary file"), getDefaultDirectory(), extensions);
    int exitCode = dialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        QString boundaryFilePath = dialog->selectedFiles().first();
        
        this->meshCoordinateSystem = dialog->isLatitudeLongitudeChecked() ? CoordinateSystem::LATITUDE_LONGITUDE : CoordinateSystem::UTM;
        ui->edtBoundaryFileLine->setText(boundaryFilePath);
        appSettings->setValue(BOUNDARY_DEFAULT_DIR_KEY, QFileInfo(boundaryFilePath).absolutePath());
    }
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
    QString extensions = "Text file (*.txt *xyz);;Keyhole Markup Language file (*.kml)";
    CoordinateFileDialog *dialog = new CoordinateFileDialog(this, tr("Select a boundary file"), getDefaultDirectory(), extensions);
    int exitCode = dialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        QString refinementFileStr = dialog->selectedFiles().first();
        CoordinateSystem coordinateSystem = dialog->isLatitudeLongitudeChecked() ? CoordinateSystem::LATITUDE_LONGITUDE : CoordinateSystem::UTM;
        
        if (!ui->lstRefinementAreas->findItems(refinementFileStr, Qt::MatchExactly).empty()) {
            QMessageBox::information(this, tr("Unstructured Mesh Generation"), tr("Coordinates file already added."));
            return;
        }
        
        try {
            // TODO: Use mesh polygon name
            currentMesh->addMeshPolygon("", refinementFileStr, MeshPolygonType::REFINEMENT_AREA, coordinateSystem);
            
            ui->lstRefinementAreas->addItem(refinementFileStr);
            ui->lstRefinementAreas->setCurrentRow(ui->lstRefinementAreas->count() - 1);
        } catch (const MeshPolygonException &ex) {
            QMessageBox::critical(this, tr("Unstructured Mesh Generation"), ex.what());
        }
        
        appSettings->setValue(BOUNDARY_DEFAULT_DIR_KEY, QFileInfo(refinementFileStr).absolutePath());
    }
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

    currentMesh->setCoordinatesDistance(ui->sbxCoordinatesDistance->value());
    
    MeshPolygon* boundaryPolygon = nullptr;
    
    CGAL::Failure_behaviour old_behaviour = CGAL::set_error_behaviour(CGAL::THROW_EXCEPTION);
    try {
        boundaryPolygon = currentMesh->addMeshPolygon(MeshPolygon::BOUNDARY_POLYGON_NAME, boundaryFileStr, MeshPolygonType::BOUNDARY, this->meshCoordinateSystem);
        currentMesh->generate();
    } catch(const MeshPolygonException &e) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), e.what());
        return;
    } catch (const std::exception&) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), tr("This triangulation does not deal with intersecting constraints."));
        return;
    }
    CGAL::set_error_behaviour(old_behaviour);
    
    ui->vtkWidget->render(currentMesh);
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

    
    currentMesh->setCoordinatesDistance(ui->sbxCoordinatesDistance->value());

    CGAL::Failure_behaviour old_behaviour = CGAL::set_error_behaviour(CGAL::THROW_EXCEPTION);
    try {
        MeshPolygon *boundaryPolygon = currentMesh->addMeshPolygon(MeshPolygon::BOUNDARY_POLYGON_NAME, boundaryFileStr, MeshPolygonType::BOUNDARY, meshCoordinateSystem);
        
        boundaryPolygon->setMinimumAngle(ui->sbxMinimumAngle->value());
        boundaryPolygon->setMaximumEdgeLength(ui->sbxMaximumEdgeLength->value());
        currentMesh->generate();
        ui->vtkWidget->render(currentMesh);
    } catch (const MeshPolygonException &e) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), e.what());
    } catch (const std::exception&) {
        QMessageBox::critical(this, tr("Unstructured Mesh Generation"), tr("This triangulation does not deal with intersecting constraints."));
    }
    CGAL::set_error_behaviour(old_behaviour);
}

void UnstructuredMeshDialog::on_cbxMeshName_currentIndexChanged(const QString &meshName) {
    if (meshName.isEmpty()) {
        return;
    }

    currentMesh = static_cast<UnstructuredMesh*>(IPHApplication::getCurrentProject()->getMesh(meshName));
    MeshPolygon *boundaryPolygon = currentMesh->getBoundaryPolygon();
    QList<MeshPolygon*> islands = currentMesh->getIslands();

    ui->edtMeshName->setText(currentMesh->getName());
    ui->edtBoundaryFileLine->setText(boundaryPolygon->getFilename());
    ui->sbxCoordinatesDistance->setValue(currentMesh->getCoordinatesDistance());
    ui->lstRefinementAreas->setCurrentRow(0);
    ui->sbxMinimumAngle->setValue(currentMesh->getBoundaryPolygon()->getMinimumAngle());
    ui->sbxMaximumEdgeLength->setValue(currentMesh->getBoundaryPolygon()->getMaximumEdgeLength());
    ui->lstIslands->clear();
    
    for (MeshPolygon *island : islands) {
        QListWidgetItem *item = new QListWidgetItem(island->getName());
        
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->lstIslands->addItem(item);
    }

    ui->vtkWidget->render(currentMesh);
}

void UnstructuredMeshDialog::on_btnNewMesh_clicked() {
    unsavedMesh->clear();
    currentMesh = unsavedMesh;
    ui->cbxMeshName->blockSignals(true);
    ui->cbxMeshName->setCurrentIndex(-1);
    ui->cbxMeshName->blockSignals(false);
    ui->edtMeshName->setText("");
    ui->edtMeshName->setFocus();
    ui->edtBoundaryFileLine->setText("");
    ui->sbxCoordinatesDistance->setValue(0);
    ui->lstIslands->clear();
    ui->vtkWidget->clear();
}

void UnstructuredMeshDialog::on_btnApplyMesh_clicked() {
    QString oldMeshName = ui->cbxMeshName->currentText();
    QString newMeshName = ui->edtMeshName->text();
    
    if (newMeshName.isEmpty()) {
        QMessageBox::warning(this, tr("Unstructured Mesh Generation"), tr("Mesh name can't be empty."));
        return;
    }
    
    currentMesh->setCoordinatesDistance(ui->sbxCoordinatesDistance->value());
    
    if (oldMeshName.isEmpty()) { // new mesh
        Project *project = IPHApplication::getCurrentProject();
        
        if (project->containsMesh(newMeshName)) {
            QMessageBox::critical(this, tr("Unstructured Mesh Generation"), tr("Mesh name already used."));
            return;
        }
        
        currentMesh->setName(newMeshName);
        project->addMesh(currentMesh);
        unsavedMesh = new UnstructuredMesh();
        
        ui->cbxMeshName->addItem(newMeshName);
        ui->cbxMeshName->setCurrentText(newMeshName);
    } else {
        currentMesh->setName(newMeshName);
        ui->cbxMeshName->setItemText(ui->cbxMeshName->currentIndex(), newMeshName);
    }
}

void UnstructuredMeshDialog::on_btnRemoveMesh_clicked() {
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