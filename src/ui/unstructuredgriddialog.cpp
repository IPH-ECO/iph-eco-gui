#include "../../include/ui/unstructuredgriddialog.h"
#include "ui_unstructuredgriddialog.h"

UnstructuredGridDialog::UnstructuredGridDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UnstructuredGridDialog)
{
    ui->setupUi(this);
}

UnstructuredGridDialog::~UnstructuredGridDialog()
{
    delete ui;
}
