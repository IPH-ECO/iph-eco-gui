#include "include/ui/grid_information_dialog.h"
#include "ui_grid_information_dialog.h"

#include "include/exceptions/grid_data_exception.h"

#include <QFileDialog>
#include <QMessageBox>

GridInformationDialog::GridInformationDialog(QDialog *parent, GridDataConfiguration *gridConfiguration, GridData *gridData, Mesh *mesh) :
    QDialog(parent), GRID_DATA_DEFAULT_DIR_KEY("grid_data_default_dir"), ui(new Ui::GridInformationDialog),
    gridConfiguration(gridConfiguration), gridData(gridData), mesh(mesh)
{
    ui->setupUi(this);

    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);

    if (gridData != nullptr) {
        mesh = gridData->getMesh();
        ui->edtName->setText(gridData->getName());
    	ui->edtInputFile->setText(gridData->getInputFile());
        ui->cbxGridInformation->setCurrentText(gridData->gridDataTypeToString());
        
        if (gridData->getGridDataInputType() == GridDataInputType::POINT) {
            ui->rdoPoint->setChecked(true);
            ui->edtExponent->setText(QString::number(gridData->getExponent()));
            ui->edtRadius->setText(QString::number(gridData->getRadius()));
        } else {
            ui->rdoPolygon->setChecked(true);
        }
    }
}

GridInformationDialog::~GridInformationDialog() {
    delete ui;
}

QString GridInformationDialog::getDefaultDirectory() {
    return appSettings->value(GRID_DATA_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(GRID_DATA_DEFAULT_DIR_KEY).toString();
}

void GridInformationDialog::on_rdoPoint_toggled(bool checked) {
    ui->edtExponent->setEnabled(checked);
    ui->edtRadius->setEnabled(checked);
}

void GridInformationDialog::on_rdoPolygon_toggled(bool checked) {
    ui->edtExponent->setEnabled(!checked);
    ui->edtRadius->setEnabled(!checked);
}

void GridInformationDialog::on_btnBrowseInputFile_clicked() {
    QString gridDataFile = QFileDialog::getOpenFileName(this, tr("Select a grid data file"), getDefaultDirectory(), tr("XYZ files (*.xyz *.txt)"));

    if (gridDataFile.isEmpty()) {
        return;
    }
    
    ui->edtInputFile->setText(gridDataFile);
    appSettings->setValue(GRID_DATA_DEFAULT_DIR_KEY, QFileInfo(gridDataFile).absolutePath());
}

GridData* GridInformationDialog::getGridData() {
	return gridData;
}

bool GridInformationDialog::isValid() {
    if (ui->edtName->text().isEmpty()) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Name can't be blank."));
        return false;
    }
    
    if (gridConfiguration->getGridData(ui->edtName->text()) != gridData) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Name already used in this configuration."));
        return false;
    }
    
    if (!ui->rdoPoint->isChecked() && !ui->rdoPolygon->isChecked()) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Please select a input type."));
        return false;
    }
    
    if (ui->edtInputFile->text().isEmpty() && (gridData == nullptr || !gridData->isPersisted())) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Please select a input file."));
        return false;
    }
    
    if (ui->cbxGridInformation->currentIndex() == - 1) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Please select a grid information option."));
        return false;
    }
    
    return true;
}

void GridInformationDialog::accept() {
    if (!isValid()) {
        return;
    }
    
    try {
        GridDataInputType gridInputType = ui->rdoPoint->isChecked() ? GridDataInputType::POINT : GridDataInputType::POLYGON;
        GridDataType gridDataType = GridData::toGridDataType(ui->cbxGridInformation->currentText());
        
        if (gridData == nullptr) {
            gridData = new GridData(mesh);
        }
        
        gridData->setName(ui->edtName->text());
        gridData->setGridDataInputType(gridInputType);
        gridData->setInputFile(ui->edtInputFile->text());
        gridData->setGridDataType(gridDataType);
        
        if (gridInputType == GridDataInputType::POINT) {
            gridData->setExponent(ui->edtExponent->text().toDouble());
            gridData->setRadius(ui->edtRadius->text().toDouble());
        }
        
        QDialog::accept();
    } catch (const GridDataException &e) {
        QMessageBox::critical(this, tr("Grid Data"), tr(e.what()));
    }
}
