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
    QString meshName = QInputDialog::getText(this, tr("Enter the mesh name"), tr("Mesh name"));

    if (!meshName.isEmpty()) {
        try {
            meshService->addUnstructuredMesh(meshName);
        } catch(MeshException &ex) {
            QMessageBox::warning(this, tr("New unstructured mesh"), ex.what());
        }
    }
}

void UnstructuredMeshWidget::on_boundaryFileBrowserButton_clicked() {
    QString boundaryFilePath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), ".", "Keyhole Markup Language file (*.kml)");

    if (!boundaryFilePath.isEmpty()) {
        QString meshName = this->ui->meshNameComboBox->currentText();

        meshService->setUnstructuredMeshBoundaryFile(meshName, boundaryFilePath);
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
