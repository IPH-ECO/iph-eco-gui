#include "include/ui/unstructured_mesh_widget.h"
#include "ui_unstructured_mesh_widget.h"

#include "include/domain/unstructured_mesh.h"

UnstructuredMeshWidget::UnstructuredMeshWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UnstructuredMeshWidget)
{
    ui->setupUi(this);
}

UnstructuredMeshWidget::~UnstructuredMeshWidget() {
    delete ui;
}

void UnstructuredMeshWidget::on_btnBoundaryFileBrowser_clicked() {
    QString boundaryFilePath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), ".", "Keyhole Markup Language file (*.kml)");

    if (!boundaryFilePath.isEmpty()) {
        ui->edtBoundaryFileLine->setText(boundaryFilePath);
    }
}

void UnstructuredMeshWidget::on_btnGenerateMesh_clicked() {
    QString meshName = ui->edtMeshName->text();

    if (meshName.isEmpty()) {
        QMessageBox::warning(this, tr("Generate Mesh"), "Please input the mesh name.");
        return;
    }

    QString boundaryFileStr = ui->edtBoundaryFileLine->text();
    QFile boundaryFile(boundaryFileStr);
    QFileInfo boundaryFileInfo(boundaryFile);

    if (!boundaryFileInfo.exists() || !boundaryFileInfo.isFile()) {
        QMessageBox::warning(this, tr("Generate Mesh"), "Boundary file not found.");
        return;
    }

    enableMeshForm(true);

    double minimumAngle = ui->sbxMinimumAngle->value();
    double maximumEdgeLength = ui->sbxMaximumEdgeLength->value();
    UnstructuredMesh unstructuredMesh(meshName, boundaryFileStr, minimumAngle, maximumEdgeLength);

    //TODO: Draw boundary on OpenGL component
    QJsonObject boundaryJson = unstructuredMesh.getBoundaryJson();
}

void UnstructuredMeshWidget::on_btnResetMesh_clicked() {
    ui->sbxMinimumAngle->setValue(ui->sbxMinimumAngle->minimum());
    ui->sbxMaximumEdgeLength->setValue(ui->sbxMaximumEdgeLength->minimum());
}

void UnstructuredMeshWidget::on_btnSaveMesh_clicked() {
    QString meshName = ui->cbxMeshName->currentIndex() == -1 ? ui->edtMeshName->text() : ui->cbxMeshName->currentText();
    QString boundaryFile = ui->edtBoundaryFileLine->text();
    double minimumAngle = ui->sbxMinimumAngle->value();
    double maximumEdgeLength = ui->sbxMaximumEdgeLength->value();

    UnstructuredMesh unstructuredMesh(meshName, boundaryFile, minimumAngle, maximumEdgeLength);
    Project *project = IPHApplication::getCurrentProject();

    UnstructuredMesh *mesh = (UnstructuredMesh*) project->getMesh(unstructuredMesh);

    if (mesh == NULL && ui->cbxMeshName->currentIndex() == -1) {
        project->addMesh(unstructuredMesh);

        ui->cbxMeshName->addItem(meshName);
        ui->cbxMeshName->setCurrentText(meshName);
    } else {
        mesh->setName(ui->edtMeshName->text());
        mesh->setBoundaryFilePath(boundaryFile);
        mesh->setMinimumAngle(minimumAngle);
        mesh->setMaximumEdgeLength(maximumEdgeLength);

        ui->cbxMeshName->setItemText(ui->cbxMeshName->currentIndex(), mesh->getName());
    }
}

void UnstructuredMeshWidget::on_btnRemoveMesh_clicked() {
    QString meshName = ui->cbxMeshName->currentText();

    if (QMessageBox::Yes == QMessageBox::question(this, tr("Remove mesh"), tr("Are you sure you want to remove '") + meshName + "'?",
                                          QMessageBox::Yes|QMessageBox::No)) {

        UnstructuredMesh unstructuredMesh(meshName);
        IPHApplication::getCurrentProject()->removeMesh(unstructuredMesh);

        ui->cbxMeshName->removeItem(ui->cbxMeshName->currentIndex());
        ui->cbxMeshName->setCurrentIndex(-1);
    }
}

void UnstructuredMeshWidget::on_btnCancelMesh_clicked() {
    resetMeshForm();
    ui->cbxMeshName->setCurrentIndex(-1);
}

void UnstructuredMeshWidget::on_cbxMeshName_currentIndexChanged(int index) {
    if (index > -1) {
        enableMeshForm(true);
        ui->btnRemoveMesh->setEnabled(true);

        QString meshName = ui->cbxMeshName->currentText();
        UnstructuredMesh unstructuredMesh(meshName);
        UnstructuredMesh *mesh = (UnstructuredMesh*) IPHApplication::getCurrentProject()->getMesh(unstructuredMesh);

        ui->edtMeshName->setText(mesh->getName());
        ui->edtBoundaryFileLine->setText(mesh->getBoundaryFilePath());
        ui->sbxMinimumAngle->setValue(mesh->getMinimumAngle());
        ui->sbxMaximumEdgeLength->setValue(mesh->getMaximumEdgeLength());
    } else {
        resetMeshForm();
    }
}

void UnstructuredMeshWidget::enableMeshForm(bool enable) {
    ui->btnRefineSpecificRegion->setEnabled(enable);
    ui->btnApplyCoarsening->setEnabled(enable);

    ui->chkShowEdges->setEnabled(enable);
    ui->chkShowTriangles->setEnabled(enable);
    ui->chkShowUtmCoordinates->setEnabled(enable);
    ui->chkShowVertexes->setEnabled(enable);

    ui->btnSaveMesh->setEnabled(enable);
    ui->btnCancelMesh->setEnabled(enable);
}

void UnstructuredMeshWidget::resetMeshForm() {
    ui->edtMeshName->setFocus();

    ui->edtMeshName->clear();
    ui->edtBoundaryFileLine->clear();
    on_btnResetMesh_clicked();

    ui->btnRemoveMesh->setEnabled(false);
    enableMeshForm(false);
}
