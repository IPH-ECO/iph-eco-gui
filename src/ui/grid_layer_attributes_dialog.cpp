#include "include/ui/grid_layer_attributes_dialog.h"
#include "ui_grid_layer_attributes_dialog.h"

#include <vtkPolyData.h>
#include <vtkCellData.h>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "include/ui/grid_data_dialog.h"

GridLayerAttributesDialog::GridLayerAttributesDialog(QWidget *parent, GridData *gridData) :
    QDialog(parent), ui(new Ui::GridLayerAttributesDialog), gridData(gridData)
{
    ui->setupUi(this);
    
    ui->edtMinimum->setText(QString::number(gridData->getMininumRange()));
    ui->edtMaximum->setText(QString::number(gridData->getMaximumRange()));
    
    double *range = gridData->getMesh()->getPolyData()->GetCellData()->GetScalars(gridData->getName().toStdString().c_str())->GetRange();
    this->defaultMinimum = range[0];
    this->defaultMaximum = range[1];
    ui->lblOriginalValues->setText(QString("[%1, %2]").arg(this->defaultMinimum).arg(this->defaultMaximum));
    ui->chkWeightBar->setChecked(gridData->getWeightBar());
    ui->chkLighting->setChecked(gridData->getLighting());
}

GridLayerAttributesDialog::~GridLayerAttributesDialog() {
    delete ui;
}

void GridLayerAttributesDialog::on_btnUseOriginalValues_clicked() {
    ui->edtMinimum->setText(QString::number(this->defaultMinimum));
    ui->edtMaximum->setText(QString::number(this->defaultMaximum));
}

void GridLayerAttributesDialog::on_buttonBox_clicked(QAbstractButton *button) {
    QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);
    
    if (standardButton == QDialogButtonBox::Cancel) {
        this->reject();
        return;
    }
    
    if (!isValid()) {
        return;
    }
    
    GridDataDialog *gridDataDialog = static_cast<GridDataDialog*>(parent());
    
    gridData->setMinimumRange(ui->edtMinimum->text().toDouble());
    gridData->setMaximumRange(ui->edtMaximum->text().toDouble());
    gridData->setWeightBar(ui->chkWeightBar->isChecked());
    gridData->setLighting(ui->chkLighting->isChecked());
    gridDataDialog->getGridDataVTKWidget()->render(gridData);
    
    if (standardButton == QDialogButtonBox::Ok) {
        this->accept();
    }
}

bool GridLayerAttributesDialog::isValid() {
    if (ui->edtMinimum->text().isEmpty()) {
        QMessageBox::warning(this, tr("Grid Layer"), tr("Minimum range can't be empty"));
        return false;
    }
    if (ui->edtMaximum->text().isEmpty()) {
        QMessageBox::warning(this, tr("Grid Layer"), tr("Maximum range can't be empty"));
        return false;
    }
    
    return true;
}
