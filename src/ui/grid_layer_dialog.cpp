#include <ui/grid_layer_dialog.h>
#include "ui_grid_layer_dialog.h"
#include <ui/coordinate_file_dialog.h>

#include <QFileDialog>
#include <QMessageBox>

GridLayerDialog::GridLayerDialog(QDialog *parent, GridDataConfiguration *gridConfiguration, GridData *gridData, Mesh *mesh) :
    QDialog(parent), GRID_DATA_DEFAULT_DIR_KEY("grid_data_default_dir"), ui(new Ui::GridLayerDialog),
    gridConfiguration(gridConfiguration), gridData(gridData), mesh(mesh)
{
    ui->setupUi(this);

	Qt::WindowFlags flags = this->windowFlags() & (~Qt::WindowContextHelpButtonHint);
	this->setWindowFlags(flags);

    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);

    if (gridData) {
        mesh = gridData->getMesh();
        ui->edtName->setText(gridData->getName());
    	ui->edtInputFile->setText(gridData->getInputFile());
        ui->cbxGridInformation->setCurrentText(GridData::getGridTypesMap().value(gridData->getGridDataType()));
        
        if (gridData->getGridDataInputType() == GridDataInputType::POINT) {
            ui->rdoPoint->setChecked(true);
            ui->edtExponent->setText(QString::number(gridData->getExponent()));
            ui->edtRadius->setText(QString::number(gridData->getRadius()));
        } else {
            ui->rdoPolygon->setChecked(true);
        }
        
        ui->edtInputFile->setEnabled(false);
        ui->rdoPoint->setEnabled(false);
        ui->rdoPolygon->setEnabled(false);
        ui->edtInputFile->setEnabled(false);
        ui->btnBrowseInputFile->setEnabled(false);
        ui->edtExponent->setEnabled(false);
        ui->edtRadius->setEnabled(false);
    }
    
    for (QString label : GridData::getGridTypesMap().values()) {
        ui->cbxGridInformation->addItem(label);
    }
}

GridLayerDialog::~GridLayerDialog() {
    delete ui;
}

QString GridLayerDialog::getDefaultDirectory() {
    return appSettings->value(GRID_DATA_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(GRID_DATA_DEFAULT_DIR_KEY).toString();
}

void GridLayerDialog::on_rdoPoint_toggled(bool checked) {
    ui->edtExponent->setEnabled(checked);
    ui->edtRadius->setEnabled(checked);
}

void GridLayerDialog::on_rdoPolygon_toggled(bool checked) {
    ui->edtExponent->setEnabled(!checked);
    ui->edtRadius->setEnabled(!checked);
}

void GridLayerDialog::on_btnBrowseInputFile_clicked() {
    CoordinateFileDialog *dialog = new CoordinateFileDialog(this, tr("Select a boundary file"), getDefaultDirectory(), tr("Text files (*.xyz *.txt)"));
    int exitCode = dialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        QString gridDataFile = dialog->selectedFiles().first();
        
        this->coordinateSystem = dialog->isLatitudeLongitudeChecked() ? CoordinateSystem::LATITUDE_LONGITUDE : CoordinateSystem::UTM;
        ui->edtInputFile->setText(gridDataFile);
        appSettings->setValue(GRID_DATA_DEFAULT_DIR_KEY, QFileInfo(gridDataFile).absolutePath());
    }
}

GridData* GridLayerDialog::getGridData() {
	return gridData;
}

bool GridLayerDialog::isValid() {
    QString gridDataName = ui->edtName->text();
    
    if (gridDataName.isEmpty()) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Name can't be blank."));
        return false;
    }
    
    if (gridConfiguration->getGridData(gridDataName) && gridData && gridDataName != gridData->getName()) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Name already used in this configuration."));
        return false;
    }
    
    if (gridConfiguration->getMesh() && gridConfiguration->getMesh()->hasArray(gridDataName)) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Name already used in the selected mesh."));
        return false;
    }
    
    if (!ui->rdoPoint->isChecked() && !ui->rdoPolygon->isChecked()) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Please select a input type."));
        return false;
    }
    
    GridDataType gridDataType = GridData::getGridTypesMap().key(ui->cbxGridInformation->currentText());
    
    if (gridDataType == GridDataType::BATHYMETRY && !gridConfiguration->getGridData(GridDataType::BATHYMETRY).isEmpty()) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Sediment level (bathymetry) layer already created."));
        return false;
    }
    
    if (gridDataType == GridDataType::ROUGHNESS && !gridConfiguration->getGridData(GridDataType::ROUGHNESS).isEmpty()) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Roughness layer already created."));
        return false;
    }
    
    if (ui->edtInputFile->text().isEmpty() && (!gridData || !gridData->isPersisted())) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Please select a input file."));
        return false;
    }
    
    if (ui->cbxGridInformation->currentIndex() == - 1) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Please select a grid information option."));
        return false;
    }
    
    return true;
}

void GridLayerDialog::accept() {
    if (!isValid()) {
        return;
    }
    
    if (!gridData) {
        gridData = new GridData(mesh);
    }
    
    GridDataInputType gridInputType = ui->rdoPoint->isChecked() ? GridDataInputType::POINT : GridDataInputType::POLYGON;
    GridDataType gridDataType = GridData::getGridTypesMap().key(ui->cbxGridInformation->currentText());
    
    gridData->setName(ui->edtName->text());
    gridData->setGridDataInputType(gridInputType);
    gridData->setInputFile(ui->edtInputFile->text());
    gridData->setGridDataType(gridDataType);
    gridData->setCoordinateSystem(coordinateSystem);

    if (gridInputType == GridDataInputType::POINT) {
        gridData->setExponent(ui->edtExponent->text().toDouble());
        gridData->setRadius(ui->edtRadius->text().toDouble());
    }
    
    QDialog::accept();
}
