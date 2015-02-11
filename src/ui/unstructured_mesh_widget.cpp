#include "include/ui/unstructured_mesh_widget.h"
#include "ui_unstructured_mesh_widget.h"

UnstructuredMeshWidget::UnstructuredMeshWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UnstructuredMeshWidget)
{
    ui->setupUi(this);
    this->meshService = new UnstructuredMeshService();
}

UnstructuredMeshWidget::~UnstructuredMeshWidget() {
    delete meshService;
    delete ui;
}

void UnstructuredMeshWidget::on_btnBoundaryFileBrowser_clicked() {
    QString boundaryFilePath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), ".", "Keyhole Markup Language file (*.kml)");

    if (!boundaryFilePath.isEmpty()) {
        QString meshName = ui->cbxMeshName->currentText();

        ui->edtBoundaryFileLine->setText(boundaryFilePath);
        meshService->setBoundaryFilePath(meshName, boundaryFilePath);
    }
}

void UnstructuredMeshWidget::on_btnGenerateMesh_clicked() {
    enableMeshForm(true);

    QFile file(ui->edtBoundaryFileLine->text());
    QFileInfo fileInfo(file);

    if (fileInfo.exists() && fileInfo.isFile()) {
        QProgressDialog *progressDialog = new QProgressDialog("Importing boundary file...", "Cancel", 0, 100, this);

        progressDialog->setAttribute(Qt::WA_DeleteOnClose, true);
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setMinimumDuration(100);

        QString meshName = ui->cbxMeshName->currentText();
        QJsonObject boundary = meshService->getBoundaryJson(meshName, progressDialog);

        if (!progressDialog->wasCanceled()) {
            //TODO: Draw boundary on OpenGL widget
        }
    } else {
        QMessageBox::warning(this, tr("Generate Mesh"), "Boundary file not found.");
    }
}

void UnstructuredMeshWidget::on_btnResetMesh_clicked() {
    ui->sbxMinimumAngle->setValue(ui->sbxMinimumAngle->minimum());
    ui->sbxMaximumEdgeLength->setValue(ui->sbxMaximumEdgeLength->minimum());
}

void UnstructuredMeshWidget::on_btnSaveMesh_clicked() {
    ui->cbxMeshName->addItem(ui->edtMeshName->text());
    ui->cbxMeshName->setCurrentIndex(-1);
    resetMeshForm();
}

void UnstructuredMeshWidget::on_btnRemoveMesh_clicked() {
    QString meshName = ui->cbxMeshName->currentText();

    if (QMessageBox::Yes == QMessageBox::question(this, tr("Remove mesh"), tr("Are you sure you want to remove '") + meshName + "'?",
                                          QMessageBox::Yes|QMessageBox::No)) {
        meshService->removeMesh(meshName);

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

        //TODO: Load strings from mesh
        ui->edtMeshName->setText(ui->cbxMeshName->currentText());
        ui->edtBoundaryFileLine->setText("TODO");
        ui->sbxMinimumAngle->setValue(0.125);
        ui->sbxMaximumEdgeLength->setValue(0.05);
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
