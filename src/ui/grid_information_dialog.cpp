#include "include/ui/grid_information_dialog.h"
#include "ui_grid_information_dialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include "include/exceptions/grid_data_exception.h"

GridInformationDialog::GridInformationDialog(QDialog *parent, GridData *gridData) :
    QDialog(parent), GRID_INFORMATION_DEFAULT_DIR_KEY("grid_data_default_dir"), ui(new Ui::GridInformationDialog), gridData(gridData)
{
    ui->setupUi(this);

    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);

    if (gridData != NULL) {
    	if (gridData->getGridInputType() == GridData::POINT) {
    		ui->rdoPoint->setChecked(true);
    	} else {
    		ui->rdoPolygon->setChecked(true);
    	}

    	ui->edtInputFile->setText(gridData->getInputFile());
        ui->cbxGridInformation->setCurrentText(gridData->getGridInformationType().toString());
        ui->edtExponent->setText(QString::number(gridData->getExponent()));
        ui->edtRadius->setText(QString::number(gridData->getRadius()));
    }
}

GridInformationDialog::~GridInformationDialog() {
    delete ui;
}

QString GridInformationDialog::getDefaultDirectory() {
    return appSettings->value(GRID_INFORMATION_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(GRID_INFORMATION_DEFAULT_DIR_KEY).toString();
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
    QString gridDataFile = QFileDialog::getOpenFileName(this, tr("Select a grid data file"), getDefaultDirectory(), "XYZ files (*.xyz), Text Files (*.txt)");

    ui->edtInputFile->setText(gridDataFile);
}

GridData* GridInformationDialog::getGridData() {
	return gridData;
}

void GridInformationDialog::on_bottomButtons_clicked(QAbstractButton *button) {
    if (ui->bottomButtons->standardButton(button) == QDialogButtonBox::Save) {
        if (!ui->rdoPoint->isChecked() && !ui->rdoPolygon->isChecked()) {
            QMessageBox::warning(this, tr("Grid Data"), tr("Please select a input type."));
            return;
        }

        if (ui->edtInputFile->text().isEmpty()) {
            QMessageBox::warning(this, tr("Grid Data"), tr("Please select a input file."));
            return;
        }

        if (ui->cbxGridInformation->currentIndex() == - 1) {
            QMessageBox::warning(this, tr("Grid Data"), tr("Please select a grid information option."));
            return;
        }

        GridData::GridInputType gridInputType = ui->rdoPoint->isChecked() ? GridData::POINT : GridData::POLYGON;
        QString inputFile = ui->edtInputFile->text();
        GridInformationType gridInformationType = GridInformationType::toGridInformationType(ui->cbxGridInformation->currentText());
        double exponent = ui->edtExponent->text().toDouble();
        double radius = ui->edtRadius->text().toDouble();
        GridData tempGridData;

        tempGridData.setGridInputType(gridInputType);
        tempGridData.setInputFile(inputFile);
        tempGridData.setGridInformationType(gridInformationType);
        tempGridData.setShow(true);

        if (gridInputType == GridData::POLYGON) {
            tempGridData.setExponent(exponent);
            tempGridData.setRadius(radius);
        }

        try {
            tempGridData.buildData();

            if (gridData == NULL) {
                if (gridInputType == GridData::POLYGON && !tempGridData.getGridDataPolygon().is_simple()) {
                    QMessageBox::critical(this, tr("Grid Data"), tr("Invalid polygon."));
                    return;
                }

                gridData = new GridData();
            }

            gridData->copy(tempGridData);

            this->accept();
        } catch (GridDataException &e) {
            QMessageBox::critical(this, tr("Grid Data"), tr(e.what()));
        }
    } else {
        this->reject();
    }
}
