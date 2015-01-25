#include "include/ui/structuredgriddialog.h"
#include "ui_structuredgriddialog.h"

StructuredGridDialog::StructuredGridDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StructuredGridDialog)
{
    ui->setupUi(this);
}

void StructuredGridDialog::on_boundaryFileButton_clicked() {
    QString filepath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), ".", "Keyhole Markup Language files (*.kml)");
    ui->boundaryFileTextEdit->setText(filepath);
}

StructuredGridDialog::~StructuredGridDialog() {
    delete ui;
}
