#include "include/ui/grid_data_dialog.h"
#include "ui_grid_data_dialog.h"

#include <QFileDialog>
#include <QTableWidgetItem>
#include <QMessageBox>

#include "include/application/iph_application.h"
#include "include/domain/structured_mesh.h"
#include "include/domain/unstructured_mesh.h"

GridDataDialog::GridDataDialog(QWidget *parent) :
    QDialog(parent), GRID_DATA_DEFAULT_DIR_KEY("grid_data_default_dir"), ui(new Ui::GridDataDialog),
    unsavedGridDataConfiguration(new GridDataConfiguration()), currentGridDataConfiguration(unsavedGridDataConfiguration),
    unsavedGridData(new GridData()), currentGridData(unsavedGridData)
{
    ui->setupUi(this);

    ui->tblGridInformation->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);
    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();

    for (QSet<Mesh*>::iterator i = meshes.begin(); i != meshes.end(); ++i) {
        ui->cbxMesh->addItem((*i)->getName());
    }
    ui->cbxMesh->setCurrentIndex(-1);

    QSet<GridDataConfiguration*> gridDataConfigurations = project->getGridDataConfigurations();
    for (QSet<GridDataConfiguration*>::const_iterator it = gridDataConfigurations.begin(); it != gridDataConfigurations.end(); it++) {
        ui->cbxConfiguration->addItem((*it)->getName());
    }
    ui->cbxConfiguration->setCurrentIndex(-1);
}

GridDataDialog::~GridDataDialog() {
    delete unsavedGridData;
    delete unsavedGridDataConfiguration;
    delete ui;
}

QString GridDataDialog::getDefaultDirectory() {
    return appSettings->value(GRID_DATA_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(GRID_DATA_DEFAULT_DIR_KEY).toString();
}

void GridDataDialog::on_cbxMesh_currentIndexChanged(const QString &meshName) {
    Project *project = IPHApplication::getCurrentProject();
    Mesh *mesh = project->getMesh(meshName);

    if (dynamic_cast<UnstructuredMesh*>(mesh) != NULL) {
        mesh = static_cast<UnstructuredMesh*>(mesh);
    } else {
        mesh = static_cast<StructuredMesh*>(mesh);
    }

    ui->gridDataOpenGLWidget->setMesh(mesh);
}

void GridDataDialog::on_btnAddGridInfomation_clicked() {
    resetForm();
}

void GridDataDialog::on_btnRemoveGridInformation_clicked() {

}

void GridDataDialog::on_rdoPoint_toggled(bool checked) {
    ui->edtExponent->setEnabled(checked);
    ui->edtRadius->setEnabled(checked);
}

void GridDataDialog::on_rdoPolygon_toggled(bool checked) {

}


void GridDataDialog::on_btnBrowse_clicked() {
    QString gridDataFile = QFileDialog::getOpenFileName(this, tr("Select a grid data file"), getDefaultDirectory(), "Keyhole Markup Language file (*.kml)");

    ui->edtInputFile->setText(gridDataFile);
}

void GridDataDialog::on_btnDone_clicked() {
    if (!ui->rdoPoint->isChecked() && !ui->rdoPolygon->isChecked()) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Select a input type."));
        return;
    }

    GridData::GridInputType gridInputType = ui->rdoPoint->isChecked() ? GridData::POINT : GridData::POLYGON;
    QString inputFile = ui->edtInputFile->text();
    GridData::GridInformationType gridInformationType = GridData::GridInformationType::toGridInformationType(ui->cbxGridInformation->currentText());

    GridData gridData(currentMesh);
    gridData.setGridInputType(gridInputType);
    gridData.setInputFile(inputFile);
    gridData.setGridInformationType(gridInformationType);
}

void GridDataDialog::on_cbxConfiguration_currentIndexChanged(const QString &configurationName) {
    ui->tblGridInformation->clear();

    Project *project = IPHApplication::getCurrentProject();
    GridDataConfiguration *gridDataConfiguration = project->getGridDataConfiguration(configurationName);
    QSet<GridData*> &gridDataSet = gridDataConfiguration->getGridDataSet();

    if (gridDataConfiguration != NULL) {
        for (QSet<GridData*>::const_iterator it = gridDataSet.begin(); it != gridDataSet.end(); it++) {
            int rowCount = ui->tblGridInformation->rowCount();

            ui->tblGridInformation->insertRow(rowCount);
            ui->tblGridInformation->setItem(rowCount, 0, (*it)->getGridInformationTypeToString());
            ui->tblGridInformation->setItem(rowCount, 1, (*it)->getGridInputTypeToString());
        }
    }
}

void GridDataDialog::resetForm() {
    ui->rdoPoint->setChecked(false);
    ui->rdoPolygon->setChecked(false);
    ui->edtInputFile->clear();
    ui->cbxGridInformation->setCurrentIndex(-1);
    ui->edtExponent->setText(ui->edtExponent->text());
    ui->edtRadius->clear();
}
