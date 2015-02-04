#include "include/ui/unstructured_grid_dialog.h"
#include "ui_unstructured_grid_dialog.h"

UnstructuredGridDialog::UnstructuredGridDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UnstructuredGridDialog)
{
    ui->setupUi(this);
}

void UnstructuredGridDialog::on_boundaryFileButton_clicked() {
    QString filepath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), ".", "Keyhole Markup Language files (*.kml)");
    ui->boundaryFileTextEdit->setText(filepath);
}

UnstructuredGridDialog::~UnstructuredGridDialog() {
    delete ui;
}
