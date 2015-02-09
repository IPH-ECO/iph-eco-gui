#include "include/ui/unstructured_mesh_widget.h"
#include "ui_unstructured_mesh_widget.h"

UnstructuredMeshWidget::UnstructuredMeshWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UnstructuredMeshWidget)
{
    ui->setupUi(this);
    this->meshService = new MeshService();
}

UnstructuredMeshWidget::~UnstructuredMeshWidget() {
    delete meshService;
    delete ui;
}

void UnstructuredMeshWidget::on_btnNewMesh_clicked() {
    QString meshName = QInputDialog::getText(this, tr("Enter the mesh name"), tr("Mesh name"));

    if (!meshName.isEmpty()) {
        try {
            meshService->addUnstructuredMesh(meshName);

            ui->cbxMeshName->addItem(meshName);
            ui->cbxMeshName->setCurrentText(meshName);

            toggleFormButtons(true);
        } catch(MeshException &ex) {
            QMessageBox::warning(this, tr("New unstructured mesh"), ex.what());
        }
    }
}

void UnstructuredMeshWidget::on_btnBoundaryFileBrowser_clicked() {
    QString boundaryFilePath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), ".", "Keyhole Markup Language file (*.kml)");

    if (!boundaryFilePath.isEmpty()) {
        QString meshName = this->ui->cbxMeshName->currentText();

        meshService->setUnstructuredMeshBoundaryFile(meshName, boundaryFilePath);
    }
}

void UnstructuredMeshWidget::on_btnResetMesh_clicked() {
    ui->sbxMinimumAngle->setValue(ui->sbxMinimumAngle->minimum());
    ui->sbxMaximumEdgeLength->setValue(ui->sbxMaximumEdgeLength->minimum());
}

void UnstructuredMeshWidget::toggleFormButtons(bool enable) {
    ui->btnEditMesh->setEnabled(enable);
    ui->btnRemoveMesh->setEnabled(enable);

    ui->cbxMeshName->setEnabled(enable);
    ui->edtBoundaryFileLine->setEnabled(enable);
    ui->btnBoundaryFileBrowser->setEnabled(enable);

    ui->sbxMaximumEdgeLength->setEnabled(enable);
    ui->sbxMinimumAngle->setEnabled(enable);

    ui->btnGenerateMesh->setEnabled(enable);
    ui->btnResetMesh->setEnabled(enable);

    ui->btnRefineSpecificRegion->setEnabled(enable);
    ui->btnApplyCoarsening->setEnabled(enable);

    ui->chkShowEdges->setEnabled(enable);
    ui->chkShowTriangles->setEnabled(enable);
    ui->chkShowUtmCoordinates->setEnabled(enable);
    ui->chkShowVertexes->setEnabled(enable);
}

void UnstructuredMeshWidget::on_btnRemoveMesh_clicked()
{
    QString meshName = ui->cbxMeshName->currentText();
    meshService->removeUnstructuredMesh(meshName);

    ui->cbxMeshName->removeItem(ui->cbxMeshName->currentIndex());

    if (ui->cbxMeshName->count() == 0) {
        toggleFormButtons(false);
    }
}
