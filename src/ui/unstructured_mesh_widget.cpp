#include "include/ui/unstructured_mesh_widget.h"
#include "ui_unstructured_mesh_widget.h"

#include <QDebug>

UnstructuredMeshWidget::UnstructuredMeshWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UnstructuredMeshWidget)
{
    ui->setupUi(this);
}

void UnstructuredMeshWidget::on_newMeshButton_clicked() {
    qDebug() << QInputDialog::getText(this, tr("Enter the mesh name"), tr("Mesh name"));
}

void UnstructuredMeshWidget::on_boundaryFileBrowserButton_clicked() {
    QString filepath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), ".", "Keyhole Markup Language files (*.kml)");
    ui->boundaryFileLineEdit->setText(filepath);
}

void UnstructuredMeshWidget::on_resetMeshButton_clicked() {
    ui->minimumAngleSpinBox->setValue(ui->minimumAngleSpinBox->minimum());
    ui->maximumEdgeLengthSpinBox->setValue(ui->maximumEdgeLengthSpinBox->minimum());
}

UnstructuredMeshWidget::~UnstructuredMeshWidget() {
    delete ui;
}
