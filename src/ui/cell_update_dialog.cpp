#include <ui/cell_update_dialog.h>
#include "ui_cell_update_dialog.h"

#include <QMessageBox>
#include <QItemSelectionModel>
#include <vtkCellData.h>

CellUpdateDialog::CellUpdateDialog(GridDataVTKWidget *gridDataVTKWidget) :
    QDialog(gridDataVTKWidget), ui(new Ui::CellUpdateDialog), gridDataVTKWidget(gridDataVTKWidget)
{
    ui->setupUi(this);
    ui->tblGridValues->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->lblSelectedCells->setText("Selected cells for layer '" + gridDataVTKWidget->currentGridData->getName() + "'");
    
    fillGridValuesTable();
}

CellUpdateDialog::~CellUpdateDialog() {
    delete ui;
}

void CellUpdateDialog::on_btnSelectAll_clicked() {
    if (ui->tblGridValues->selectionModel()->selectedRows().size() == ui->tblGridValues->rowCount()) {
        ui->tblGridValues->setCurrentCell(-1, -1);
    } else {
        ui->tblGridValues->selectAll();
    }
}

void CellUpdateDialog::on_btnUpdate_clicked() {
    if (ui->edtAllCellsValues->text().isEmpty()) {
        return;
    }
    
    double weight = ui->edtAllCellsValues->text().toDouble();
    QItemSelectionModel *selectionModel = ui->tblGridValues->selectionModel();
    
    for (int i = 0; i < selectionModel->selectedRows().size(); i++) {
        int row = selectionModel->selectedRows().at(i).row();
        ui->tblGridValues->item(row, 1)->setText(QString::number(weight));
    }
}

void CellUpdateDialog::on_buttonBox_clicked(QAbstractButton *button) {
    QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);
    if (standardButton == QDialogButtonBox::Ok) {
        Mesh *mesh = gridDataVTKWidget->currentMesh;
        
        for (int i = 0; i < ui->tblGridValues->rowCount(); i++) {
            vtkIdType cellId = ui->tblGridValues->item(i, 0)->text().toInt();
            double weight = ui->tblGridValues->item(i, 1)->text().toDouble();
            
            mesh->getMeshPolyData()->GetCellData()->GetScalars()->SetTuple1(cellId, weight);
        }
        
        mesh->getMeshPolyData()->Modified();
        this->accept();
    } else if (standardButton == QDialogButtonBox::Reset) {
        fillGridValuesTable();
    } else {
        this->reject();
    }
}

void CellUpdateDialog::fillGridValuesTable() {
    Mesh *mesh = gridDataVTKWidget->currentMesh;
    vtkIdTypeArray *cellIds = gridDataVTKWidget->selectedCellIds;
    
    ui->tblGridValues->setRowCount(0);
    
    for (vtkIdType i = 0; i < cellIds->GetNumberOfTuples(); i++) {
        vtkIdType cellId = cellIds->GetTuple1(i);
        double cellWeight = mesh->getMeshPolyData()->GetCellData()->GetScalars()->GetTuple1(cellId);
        int rowCount = ui->tblGridValues->rowCount();
        QTableWidgetItem *cellIdItem = new QTableWidgetItem(QString::number(cellId));
        
        cellIdItem->setFlags(cellIdItem->flags() & ~Qt::ItemIsEditable);
        ui->tblGridValues->insertRow(rowCount);
        ui->tblGridValues->setItem(rowCount, 0, cellIdItem);
        ui->tblGridValues->setItem(rowCount, 1, new QTableWidgetItem(QString::number(cellWeight)));
    }
}