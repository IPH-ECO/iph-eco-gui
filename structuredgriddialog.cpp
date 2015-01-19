#include "structuredgriddialog.h"
#include "ui_structuredgriddialog.h"

StructuredGridDialog::StructuredGridDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StructuredGridDialog)
{
    ui->setupUi(this);
}

StructuredGridDialog::~StructuredGridDialog()
{
    delete ui;
}

void StructuredGridDialog::on_boundaryFileButton_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), ".", "XY files (*.xy)");
    ui->boundaryFileTextEdit->setText(filepath);
}
