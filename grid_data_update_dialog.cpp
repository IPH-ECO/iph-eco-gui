#include "grid_data_update_dialog.h"
#include "ui_grid_data_update_dialog.h"

GridDataUpdateDialog::GridDataUpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GridDataUpdateDialog)
{
    ui->setupUi(this);
}

GridDataUpdateDialog::~GridDataUpdateDialog()
{
    delete ui;
}
