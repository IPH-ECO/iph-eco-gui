#include "include/ui/cell_update_dialog.h"
#include "ui_cell_update_dialog.h"

#include <QMessageBox>
#include <vtkCellData.h>

CellUpdateDialog::CellUpdateDialog(GridDataVTKWidget *gridDataVTKWidget) :
    QDialog(gridDataVTKWidget), ui(new Ui::CellUpdateDialog)
{
    ui->setupUi(this);
    ui->tblGridValues->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->lblSelectedCells->setText("Selected cells for layer '" + gridDataVTKWidget->currentGridData->getName() + "'");
    
    Mesh *mesh = gridDataVTKWidget->currentMesh;
    vtkIdTypeArray *cellIds = gridDataVTKWidget->selectedCellIds;
    
    for (vtkIdType i = 0; i < cellIds->GetNumberOfTuples(); i++) {
        vtkIdType cellId = cellIds->GetTuple1(i);
        double cellWeight = mesh->getPolyData()->GetCellData()->GetScalars()->GetTuple1(cellId);
        int rowCount = ui->tblGridValues->rowCount();
        QTableWidgetItem *cellIdItem = new QTableWidgetItem(QString::number(cellId));
        
        cellIdItem->setFlags(cellIdItem->flags() & ~Qt::ItemIsEditable);
        ui->tblGridValues->insertRow(rowCount);
        ui->tblGridValues->setItem(rowCount, 0, cellIdItem);
        ui->tblGridValues->setItem(rowCount, 1, new QTableWidgetItem(QString::number(cellWeight)));
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