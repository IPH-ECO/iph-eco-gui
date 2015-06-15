#include "include/ui/grid_data_dialog.h"
#include "ui_grid_data_dialog.h"

#include "include/application/iph_application.h"
#include "include/domain/structured_mesh.h"
#include "include/domain/unstructured_mesh.h"
#include "include/exceptions/grid_data_exception.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QVector>
#include <QObject>
#include <QProgressDialog>

GridDataDialog::GridDataDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::GridDataDialog),
    unsavedConfiguration(new GridDataConfiguration()), currentConfiguration(unsavedConfiguration), currentMesh(nullptr)
{
    ui->setupUi(this);
    ui->tblGridInformation->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();
    QSet<GridDataConfiguration*> configurations = project->getGridDataConfigurations();

    for (QSet<Mesh*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it) {
        ui->cbxMesh->addItem((*it)->getName());
    }
    ui->cbxMesh->setCurrentIndex(-1);

    for (QSet<GridDataConfiguration*>::const_iterator it = configurations.begin(); it != configurations.end(); it++) {
        ui->cbxConfiguration->addItem((*it)->getName());
    }
    ui->cbxConfiguration->setCurrentIndex(-1);
    
    ui->gridDataVTKWidget->clear();
}

GridDataDialog::~GridDataDialog() {
    delete unsavedConfiguration;
    delete ui;
}

void GridDataDialog::setCoordinate(double &x, double &y) {
    QString xStr = QString::number(x, 'f', 6);
    QString yStr = QString::number(y, 'f', 6);

    ui->lblUTMCoordinate->setText(QString("Easting: %1, Northing: %2").arg(xStr).arg(yStr));
}

void GridDataDialog::setArea(const double &area) {
    ui->lblDomainArea->setText(QString("Area: %1 m\u00B2").arg(area, 0, 'f', 3));
}

void GridDataDialog::on_cbxConfiguration_currentIndexChanged(const QString &configurationName) {
    ui->tblGridInformation->setRowCount(0);
    ui->gridDataVTKWidget->clear();
    
    if (configurationName.isEmpty()) {
        toggleGridDataConfigurationForm(false);
        ui->edtConfigurationName->clear();
        ui->cbxMesh->setCurrentIndex(-1);
    } else {
        Project *project = IPHApplication::getCurrentProject();
        currentConfiguration = project->getGridDataConfiguration(configurationName);
        QVector<GridData*> gridDataVector = currentConfiguration->getGridDataVector();
        currentMesh = gridDataVector.at(0)->getMesh();

        ui->edtConfigurationName->setText(currentConfiguration->getName());
        ui->cbxMesh->setCurrentText(currentConfiguration->getMesh()->getName());

        for (int i = 0; i < gridDataVector.count(); i++) {
            int rowCount = ui->tblGridInformation->rowCount();
            GridData *gridData = gridDataVector.at(i);

            ui->tblGridInformation->insertRow(rowCount);
            ui->tblGridInformation->setItem(rowCount, 0, new QTableWidgetItem(gridData->getName()));
            ui->tblGridInformation->setItem(rowCount, 1, new QTableWidgetItem(gridData->gridDataInputTypeToString()));
            ui->tblGridInformation->setItem(rowCount, 2, new QTableWidgetItem(gridData->gridDataTypeToString()));
            ui->tblGridInformation->setItem(rowCount, 3, new QTableWidgetItem(QString::number(gridData->getInputPolyData()->GetNumberOfPoints())));
        }

        ui->gridDataVTKWidget->render(currentMesh);
        ui->btnShowGridDataPoints->setEnabled(true);
        ui->btnShowGridDataPoints->toggled(false);
        ui->btnShowInterpolationResult->setEnabled(true);
        ui->btnShowInterpolationResult->toggled(true);
        toggleGridDataConfigurationForm(true);
    }
}

void GridDataDialog::on_cbxMesh_currentIndexChanged(const QString &meshName) {
    bool isMeshNamePresent = !meshName.isEmpty();

    if (isMeshNamePresent) {
        Project *project = IPHApplication::getCurrentProject();
        currentMesh = project->getMesh(meshName);

        if (currentMesh->instanceOf("UnstructuredMesh")) {
            currentMesh = static_cast<UnstructuredMesh*>(currentMesh);
        } else {
            currentMesh = static_cast<StructuredMesh*>(currentMesh);
        }
    }

    ui->gridDataVTKWidget->render(currentMesh);
    ui->btnAddGridInfomation->setEnabled(isMeshNamePresent);
    ui->btnRemoveGridInformation->setEnabled(isMeshNamePresent);
//    ui->btnShowGridDataPoints->setEnabled(isMeshNamePresent);
//    ui->btnShowGridDataPoints->toggled(false);
//    ui->btnShowInterpolationResult->setEnabled(isMeshNamePresent);
//    ui->btnShowInterpolationResult->toggled(false);
    ui->btnShowMesh->setEnabled(isMeshNamePresent);
    ui->btnShowMesh->setChecked(isMeshNamePresent);
}

void GridDataDialog::on_btnAddGridInfomation_clicked() {
    GridInformationDialog *gridInformationDialog = new GridInformationDialog(this, currentConfiguration, nullptr, currentMesh);
    int exitCode = gridInformationDialog->exec();

    if (exitCode == QDialog::Accepted) {
        int rowCount = ui->tblGridInformation->rowCount();
        GridData *gridData = gridInformationDialog->getGridData();
        int maximum = gridData->getMesh()->getPolyData()->GetNumberOfCells();
        
        QProgressDialog *progressDialog = new QProgressDialog(tr("Interpolating grid data..."), tr("Cancel"), 0, maximum - 1, this);
        QObject::connect(gridData, SIGNAL(updateProgress(int)), progressDialog, SLOT(setValue(int)));
        QObject::connect(progressDialog, SIGNAL(canceled()), gridData, SLOT(cancelInterpolation()));
        progressDialog->setMinimumDuration(500);
        progressDialog->setWindowModality(Qt::WindowModal);

        gridData->interpolate();
        
        if (progressDialog->wasCanceled()) {
            gridData->cancelInterpolation(false); // Set false for future computations
        } else {
            currentConfiguration->addGridData(gridData);
            
            ui->btnShowGridDataPoints->setEnabled(true);
            ui->btnShowGridDataPoints->setChecked(false);
            ui->btnShowInterpolationResult->setEnabled(true);
            ui->btnShowInterpolationResult->setChecked(false);
            ui->tblGridInformation->insertRow(rowCount);
            ui->tblGridInformation->setItem(rowCount, 0, new QTableWidgetItem(gridData->getName()));
            ui->tblGridInformation->setItem(rowCount, 1, new QTableWidgetItem(gridData->gridDataInputTypeToString()));
            ui->tblGridInformation->setItem(rowCount, 2, new QTableWidgetItem(gridData->gridDataTypeToString()));
            ui->tblGridInformation->setItem(rowCount, 3, new QTableWidgetItem(QString::number(gridData->getInputPolyData()->GetNumberOfPoints())));
            ui->gridDataVTKWidget->render(gridData);
        }
        delete progressDialog;
    }
}

void GridDataDialog::on_tblGridInformation_itemDoubleClicked(QTableWidgetItem *item) {
    GridData *gridData = currentConfiguration->getGridData(item->row());
    GridInformationDialog *gridInformationDialog = new GridInformationDialog(this, currentConfiguration, gridData);
    int exitCode = gridInformationDialog->exec();

    if (exitCode == QDialog::Accepted) {
        QTableWidgetItem *inputTypeItem = ui->tblGridInformation->item(item->row(), 0);
        QTableWidgetItem *gridInformationItem = ui->tblGridInformation->item(item->row(), 1);

        inputTypeItem->setText(gridData->gridDataInputTypeToString());
        gridInformationItem->setText(gridData->gridDataTypeToString());
    }
}

void GridDataDialog::on_tblGridInformation_itemClicked(QTableWidgetItem *item) {
    QString gridDataName = ui->tblGridInformation->item(item->row(), 0)->text();
    GridData *gridData = currentConfiguration->getGridData(gridDataName);
    
    currentMesh->setActiveScalars(gridDataName);
    ui->gridDataVTKWidget->setShowInterpolationResult(true);
    ui->btnShowInterpolationResult->setChecked(true);
    ui->gridDataVTKWidget->render(gridData);
    ui->btnEditGridInformation->setEnabled(true);
}

void GridDataDialog::on_btnRemoveGridInformation_clicked() {
    int currentRow = ui->tblGridInformation->currentRow();

    if (currentRow > -1 && QMessageBox::question(this, tr("Grid Data"), tr("Are you sure?")) == QMessageBox::Yes) {
        currentConfiguration->removeGridData(currentRow);
        ui->tblGridInformation->removeRow(currentRow);
        if (ui->tblGridInformation->rowCount() == 0) {
            ui->btnShowGridDataPoints->setEnabled(false);
            ui->btnShowGridDataPoints->toggled(false);
            ui->btnShowInterpolationResult->setEnabled(false);
            ui->btnShowInterpolationResult->toggled(false);
        }
        ui->gridDataVTKWidget->clear(); // FIX: Must update the map
    }
}

void GridDataDialog::toggleGridDataConfigurationForm(bool enable) {
    ui->btnDoneConfiguration->setEnabled(enable);
    ui->btnRemoveConfiguration->setEnabled(enable);
    ui->btnSaveAsNewConfiguration->setEnabled(enable);
}

void GridDataDialog::on_btnSaveConfiguration_clicked() {
    QString configurationName = ui->cbxConfiguration->currentIndex() == -1 ? ui->edtConfigurationName->text() : ui->cbxConfiguration->currentText();
    
    if (!isConfigurationValid(configurationName)) {
        return;
    }

    Project *project = IPHApplication::getCurrentProject();
    
    currentConfiguration->setName(configurationName);

    if (ui->cbxConfiguration->currentIndex() == -1) {
        project->addGridDataConfiguration(currentConfiguration);
        unsavedConfiguration = new GridDataConfiguration();

        ui->cbxConfiguration->addItem(configurationName);
        ui->cbxConfiguration->setCurrentText(configurationName);
    } else {
        ui->cbxConfiguration->setItemText(ui->cbxConfiguration->currentIndex(), configurationName);
    }
}

void GridDataDialog::on_btnSaveAsNewConfiguration_clicked() {
    QString newConfigurationName = QInputDialog::getText(this, tr("Save Grid Data Configuration as New"), tr("Configuration Name"));
    
    if (newConfigurationName.isEmpty() || isConfigurationValid(newConfigurationName)) {
        return;
    }

    GridDataConfiguration *newConfiguration = new GridDataConfiguration();
    Project *project = IPHApplication::getCurrentProject();
    QVector<GridData*> gridDataVector = currentConfiguration->getGridDataVector();

    newConfiguration->setName(newConfigurationName);

    for (int i = 0; i < gridDataVector.count(); i++) {
        GridData *gridData = gridDataVector.at(i);
        GridData *newGridData = new GridData(currentMesh);

        newGridData->setGridDataInputType(gridData->getGridDataInputType());
        newGridData->setInputFile(gridData->getInputFile());
        newGridData->setGridDataType(gridData->getGridDataType());
        newGridData->setExponent(gridData->getExponent());
        newGridData->setRadius(gridData->getRadius());

        newConfiguration->addGridData(newGridData);
    }

    project->addGridDataConfiguration(newConfiguration);
    currentConfiguration = newConfiguration;

    ui->cbxConfiguration->addItem(newConfigurationName);
    ui->cbxConfiguration->setCurrentText(newConfigurationName);
}

void GridDataDialog::on_btnRemoveConfiguration_clicked() {
    QString configurationName = ui->cbxConfiguration->currentText();

    if (QMessageBox::Yes == QMessageBox::question(this, tr("Remove grid data"), tr("Are you sure you want to remove '") + configurationName + "'?")) {
        IPHApplication::getCurrentProject()->removeGridDataConfiguration(configurationName);

        ui->cbxConfiguration->removeItem(ui->cbxConfiguration->currentIndex());
        ui->cbxConfiguration->setCurrentIndex(-1);
        ui->gridDataVTKWidget->clear();
    }
}

void GridDataDialog::on_btnDoneConfiguration_clicked() {
    ui->cbxConfiguration->setCurrentIndex(-1);
}

bool GridDataDialog::isConfigurationValid(const QString &configurationName) {
    if (configurationName.isEmpty()) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Please input the configuration name."));
        return false;
    }
    
    if (ui->cbxConfiguration->findText(configurationName, Qt::MatchFixedString) != -1) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Configuration name already used."));
        return false;
    }

    if (ui->cbxMesh->currentIndex() == -1) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Associate a mesh to this configuration."));
        return false;
    }

    if (ui->tblGridInformation->rowCount() == 0) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Please input at least one grid information."));
        return false;
    }

    return true;
}

void GridDataDialog::on_btnShowGridDataPoints_clicked(bool checked) {
    ui->gridDataVTKWidget->setShowGridDataPoints(checked);
}

void GridDataDialog::on_btnShowInterpolationResult_clicked(bool checked) {
    ui->gridDataVTKWidget->setShowInterpolationResult(checked);
}

void GridDataDialog::on_btnShowMesh_clicked(bool checked) {
    ui->gridDataVTKWidget->setShowMesh(checked);
}
