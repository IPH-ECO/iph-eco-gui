#include "include/ui/cell_update_dialog.h"
#include "ui_cell_update_dialog.h"

#include <QMessageBox>

CellUpdateDialog::CellUpdateDialog(QWidget *parent, QSet<CellInfo*> &cellInfoSet) :
    QDialog(parent), ui(new Ui::CellUpdateDialog), cellInfoSet(cellInfoSet)
{
    ui->setupUi(this);
    ui->tblGridInformation->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (QSet<CellInfo*>::const_iterator it = cellInfoSet.begin(); it != cellInfoSet.end(); it++) {
        int rowCount = ui->tblGridInformation->rowCount();
        QTableWidgetItem *gridInformationItem = new QTableWidgetItem((*it)->getGridInformationType().toString());

        gridInformationItem->setFlags(gridInformationItem->flags() & ~Qt::ItemIsEditable);
        ui->tblGridInformation->insertRow(rowCount);
        ui->tblGridInformation->setItem(rowCount, 0, gridInformationItem);
        ui->tblGridInformation->setItem(rowCount, 1, new QTableWidgetItem(QString::number((*it)->getWeight())));
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

    for (int i = 0; i < ui->tblGridInformation->rowCount(); i++) {
        QString gridInformationStr = ui->tblGridInformation->item(i, 0)->text();
        double weight = ui->tblGridInformation->item(i, 1)->text().toDouble();

        cellInfoList.push_back(CellInfo(GridInformationType::toGridInformationType(gridInformationStr), weight));
    }

    return cellInfoList;
}

void CellUpdateDialog::on_tblGridInformation_cellChanged(int row, int column) {
//    for (int i = 0; i < ui->tblGridInformation->rowCount(); i++) {
//        for (QSet<CellInfo*>::iterator it = cellInfoSet.begin(); it != cellInfoSet.end(); it++) {
//            (*it)->
//        }
//    }
}
