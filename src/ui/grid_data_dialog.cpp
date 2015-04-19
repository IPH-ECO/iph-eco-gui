#include "include/ui/grid_data_dialog.h"
#include "ui_grid_data_dialog.h"

#include <QFileDialog>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QVector>
#include <QCheckBox>
#include <QObject>
#include <QHBoxLayout>

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
    int currentRow = ui->tblGridInformation->currentRow();

    if (currentRow > -1 && QMessageBox::Yes == QMessageBox::question(this, tr("Grid Data"), tr("Are you sure?"))) {
        currentGridDataConfiguration->removeGridData(currentRow);
        ui->tblGridInformation->removeRow(currentRow);
        resetForm();
    }
}

void GridDataDialog::on_rdoPoint_toggled(bool checked) {
    ui->edtExponent->setEnabled(checked);
    ui->edtRadius->setEnabled(checked);
}

void GridDataDialog::on_rdoPolygon_toggled(bool checked) {
    ui->edtExponent->setEnabled(!checked);
    ui->edtRadius->setEnabled(!checked);
}

void GridDataDialog::on_btnBrowse_clicked() {
    QString gridDataFile = QFileDialog::getOpenFileName(this, tr("Select a grid data file"), getDefaultDirectory(), "Keyhole Markup Language file (*.kml)");

    ui->edtInputFile->setText(gridDataFile);
}

void GridDataDialog::on_btnSave_clicked() {
    if (!ui->rdoPoint->isChecked() && !ui->rdoPolygon->isChecked()) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Select a input type."));
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

    Project *project = IPHApplication::getCurrentProject();
    QString meshName = ui->cbxMesh->currentText();
    Mesh *mesh = project->getMesh(meshName);
    bool updateGridData = currentGridData != unsavedGridData;

    GridData *gridData = updateGridData ? currentGridData : (new GridData(mesh));
    GridData::GridInputType gridInputType = ui->rdoPoint->isChecked() ? GridData::POINT : GridData::POLYGON;
    QString inputFile = ui->edtInputFile->text();
    GridData::GridInformationType gridInformationType = GridData::toGridInformationType(ui->cbxGridInformation->currentText());
    double exponent = ui->edtExponent->text().toDouble();
    double radius = ui->edtRadius->text().toDouble();

    gridData->setGridInputType(gridInputType);
    gridData->setInputFile(inputFile);
    gridData->setGridInformationType(gridInformationType);
    gridData->setExponent(exponent);
    gridData->setRadius(radius);

    if (updateGridData) {
        int currentRow = ui->tblGridInformation->currentRow();
        QTableWidgetItem *gridInformationItem = ui->tblGridInformation->item(currentRow, 0);
        QTableWidgetItem *inputTypeItem = ui->tblGridInformation->item(currentRow, 1);

        gridInformationItem->setText(gridData->gridInformationTypeToString());
        inputTypeItem->setText(gridData->gridInputTypeToString());
    } else {
        currentGridDataConfiguration->addGridData(gridData);

        int rowCount = ui->tblGridInformation->rowCount();
        QWidget *checkBoxWidget = createCheckBoxWidget(gridData);

        ui->tblGridInformation->insertRow(rowCount);
        ui->tblGridInformation->setItem(rowCount, 0, new QTableWidgetItem(gridData->gridInformationTypeToString()));
        ui->tblGridInformation->setItem(rowCount, 1, new QTableWidgetItem(gridData->gridInputTypeToString()));
        ui->tblGridInformation->setCellWidget(rowCount, 2, checkBoxWidget);

        resetForm();
    }
}

void GridDataDialog::on_cbxConfiguration_currentIndexChanged(const QString &configurationName) {
    ui->tblGridInformation->clear();

    Project *project = IPHApplication::getCurrentProject();
    GridDataConfiguration *gridDataConfiguration = project->getGridDataConfiguration(configurationName);
    QVector<GridData*> &gridDataVector = gridDataConfiguration->getGridDataVector();

    if (gridDataConfiguration != NULL) {
        for (int i = 0; i < gridDataVector.count(); i++) {
            int rowCount = ui->tblGridInformation->rowCount();
            GridData *gridData = gridDataVector.at(i);

            ui->tblGridInformation->insertRow(rowCount);
            ui->tblGridInformation->setItem(rowCount, 0, new QTableWidgetItem(gridData->gridInformationTypeToString()));
            ui->tblGridInformation->setItem(rowCount, 1, new QTableWidgetItem(gridData->gridInputTypeToString()));
        }
    }
}

QWidget* GridDataDialog::createCheckBoxWidget(GridData *gridData) {
    QWidget *widget = new QWidget(ui->tblGridInformation);
    QCheckBox *checkBox = new QCheckBox();
    QHBoxLayout *layout = new QHBoxLayout(widget);

    layout->addWidget(checkBox);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0,0,0,0);
    widget->setLayout(layout);

    QObject::connect(checkBox, SIGNAL(clicked(bool)), gridData, SLOT(setShow(bool)));

    return widget;
}

void GridDataDialog::resetForm() {
    ui->tblGridInformation->clearSelection();
    ui->rdoPoint->setChecked(false);
    ui->rdoPolygon->setChecked(false);
    ui->edtInputFile->clear();
    ui->cbxGridInformation->setCurrentIndex(-1);
    ui->edtExponent->setText("2");
    ui->edtRadius->setText("10");
    ui->btnSave->setText("Save");
    currentGridData = unsavedGridData;
}

void GridDataDialog::on_tblGridInformation_itemSelectionChanged() {
    int currentRow = ui->tblGridInformation->currentRow();

    if (currentRow > -1) {
        currentGridData = currentGridDataConfiguration->getGridData(currentRow);

        ui->rdoPoint->setChecked(currentGridData->getGridInputType() == GridData::POINT);
        ui->rdoPolygon->setChecked(currentGridData->getGridInputType() == GridData::POLYGON);
        ui->edtInputFile->setText(currentGridData->getInputFile());
        ui->cbxGridInformation->setCurrentText(currentGridData->gridInformationTypeToString());
        ui->edtExponent->setText(QString::number(currentGridData->getExponent()));
        ui->edtRadius->setText(QString::number(currentGridData->getRadius()));
        ui->btnSave->setText("Update");
    }
}
