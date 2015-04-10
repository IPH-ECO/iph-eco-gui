#include "include/ui/grid_data_dialog.h"
#include "ui_grid_data_dialog.h"

GridDataDialog::GridDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GridDataDialog)
{
    ui->setupUi(this);
}

GridDataDialog::~GridDataDialog()
{
    delete ui;
}
