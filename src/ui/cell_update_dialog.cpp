#include "include/ui/cell_update_dialog.h"
#include "ui_cell_update_dialog.h"

#include <QMessageBox>

CellUpdateDialog::CellUpdateDialog(QWidget *parent, QSet<CellInfo*> &cellInfoSet) :
    QDialog(parent), ui(new Ui::CellUpdateDialog), cellInfoSet(cellInfoSet)
{
    ui->setupUi(this);
    ui->tblGridValues->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (QSet<CellInfo*>::const_iterator it = cellInfoSet.begin(); it != cellInfoSet.end(); it++) {
        int rowCount = ui->tblGridValues->rowCount();
        QTableWidgetItem *gridInformationItem = new QTableWidgetItem((*it)->getGridInformationType().toString());

        gridInformationItem->setFlags(gridInformationItem->flags() & ~Qt::ItemIsEditable);
        ui->tblGridValues->insertRow(rowCount);
        ui->tblGridValues->setItem(rowCount, 0, gridInformationItem);
        ui->tblGridValues->setItem(rowCount, 1, new QTableWidgetItem(QString::number((*it)->getWeight())));
    }
}

CellUpdateDialog::~CellUpdateDialog() {
    delete ui;
}

void CellUpdateDialog::on_buttonBox_clicked(QAbstractButton *button) {
    if (ui->buttonBox->standardButton(button) == QDialogButtonBox::Save) {
        this->accept();
    } else {
        this->reject();
    }
}

QList<CellInfo> CellUpdateDialog::getCellInfoList() {
    QList<CellInfo> cellInfoList;

    for (int i = 0; i < ui->tblGridValues->rowCount(); i++) {
        QString gridInformationStr = ui->tblGridValues->item(i, 0)->text();
        double weight = ui->tblGridValues->item(i, 1)->text().toDouble();

        cellInfoList.push_back(CellInfo(GridInformationType::toGridInformationType(gridInformationStr), weight));
    }

    return cellInfoList;
}
