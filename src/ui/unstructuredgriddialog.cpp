#include "../../include/ui/unstructuredgriddialog.h"
#include "ui_unstructuredgriddialog.h"

IPH::UI::UnstructuredGridDialog::UnstructuredGridDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UnstructuredGridDialog)
{
    ui->setupUi(this);
}

void IPH::UI::UnstructuredGridDialog::on_boundaryFileButton_clicked() {
    QString filepath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), ".", "Keyhole Markup Language files (*.kml)");
    ui->boundaryFileTextEdit->setText(filepath);
}

IPH::UI::UnstructuredGridDialog::~UnstructuredGridDialog() {
    delete ui;
}
