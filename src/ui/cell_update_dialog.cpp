#include "include/ui/cell_update_dialog.h"
#include "ui_cell_update_dialog.h"

#include <QMessageBox>

CellUpdateDialog::CellUpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CellUpdateDialog)
{
    ui->setupUi(this);
}

CellUpdateDialog::~CellUpdateDialog() {
    delete ui;
}

void CellUpdateDialog::setValues(QString gridInformationType, double weight) {
    ui->lblGridInformation->setText(gridInformationType);
    ui->edtWeight->setText(QString::number(weight));
}

double CellUpdateDialog::getWeight() {
    return ui->edtWeight->text().toDouble();
}

void CellUpdateDialog::on_buttonBox_clicked(QAbstractButton *button) {
    if (ui->buttonBox->standardButton(button) == QDialogButtonBox::Save) {
        if (ui->edtWeight->text().isEmpty()) {
            QMessageBox::warning(this, tr("Grid Data"), tr("Invalid value."));
            return;
        }

        this->accept();
    } else {
        this->reject();
    }
}
