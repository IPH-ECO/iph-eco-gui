#include "include/ui/unstructured_mesh_widget.h"
#include "ui_unstructured_mesh_widget.h"

#include <QDebug>

UnstructuredMeshWidget::UnstructuredMeshWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UnstructuredMeshWidget)
{
    ui->setupUi(this);
    this->meshService = new MeshService();
}

void UnstructuredMeshWidget::on_newMeshButton_clicked() {
    this->currentMeshName = QInputDialog::getText(this, tr("Enter the mesh name"), tr("Mesh name"));

    if (!this->currentMeshName.isEmpty()) {
        try {
            meshService->addUnstructuredMesh(this->currentMeshName);
        } catch(MeshException &ex) {
            QMessageBox::warning(this, tr("New unstructured mesh"), ex.what());
        }
    }
}

void UnstructuredMeshWidget::on_boundaryFileBrowserButton_clicked() {
    QString boundaryFilePath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), ".", "Keyhole Markup Language file (*.kml)");

    if (!boundaryFilePath.isEmpty()) {
        ui->boundaryFileLineEdit->setText(boundaryFilePath);
        meshService->setUnstructuredMeshBoundaryFile(this->currentMeshName, boundaryFilePath);
    }
}

void UnstructuredMeshWidget::on_generateMeshButton_clicked() {
    QProgressDialog *progressDialog = new QProgressDialog("Importing boundary file...", "Cancel", 0, 100, this);

    progressDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(100);

    QJsonObject boundary = meshService->getUnstructuredMeshBoundary(this->currentMeshName, progressDialog);

    if (!progressDialog->wasCanceled()) {
        //TODO: Draw boundary on OpenGL widget
    }
}

void UnstructuredMeshWidget::on_resetMeshButton_clicked() {
    ui->minimumAngleSpinBox->setValue(ui->minimumAngleSpinBox->minimum());
    ui->maximumEdgeLengthSpinBox->setValue(ui->maximumEdgeLengthSpinBox->minimum());
}

UnstructuredMeshWidget::~UnstructuredMeshWidget() {
    delete meshService;
    delete ui;
}
