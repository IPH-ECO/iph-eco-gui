#include "include/ui/grid_data_dialog.h"
#include "ui_grid_data_dialog.h"

#include "include/application/iph_application.h"
#include "include/domain/structured_mesh.h"
#include "include/domain/unstructured_mesh.h"
#include "include/exceptions/grid_data_exception.h"
#include "include/ui/grid_layer_dialog.h"
#include "include/ui/grid_layer_attributes_dialog.h"

#include <QProgressDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QVector>
#include <QObject>
#include <QPixmap>

GridDataDialog::GridDataDialog(QWidget *parent) :
    QDialog(parent), GRID_DATA_DEFAULT_DIR_KEY("grid_data_default_dir"), ui(new Ui::GridDataDialog),
    unsavedConfiguration(new GridDataConfiguration()), currentConfiguration(unsavedConfiguration), currentMesh(nullptr)
{
    ui->setupUi(this);
    
    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);

	Qt::WindowFlags flags = this->windowFlags() | Qt::WindowMaximizeButtonHint;
	this->setWindowFlags(flags);
    ui->tblGridLayers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();
    QSet<GridDataConfiguration*> configurations = project->getGridDataConfigurations();

    ui->cbxMesh->blockSignals(true);
    for (QSet<Mesh*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it) {
        ui->cbxMesh->addItem((*it)->getName());
    }
    ui->cbxMesh->setCurrentIndex(-1);
    ui->cbxMesh->blockSignals(false);

    ui->cbxConfiguration->blockSignals(true);
    for (QSet<GridDataConfiguration*>::const_iterator it = configurations.begin(); it != configurations.end(); it++) {
        ui->cbxConfiguration->addItem((*it)->getName());
    }
    ui->cbxConfiguration->setCurrentIndex(-1);
    ui->cbxConfiguration->blockSignals(false);
    
    ui->gridDataVTKWidget->clear();
    
    QColor color = QColor(Qt::white);
    QPixmap px(24, 24);
    
    px.fill(color);
    ui->btnBackgroundColor->setIcon(px);
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
	QString areaStr("Area: ");

	if (area == 0.0) {
		areaStr += "-";
	} else {
		areaStr += QString::number(area, 'f', 2) + " m\u00B2";
	}

    ui->lblDomainArea->setText(areaStr);
}

GridDataVTKWidget* GridDataDialog::getGridDataVTKWidget() {
    return ui->gridDataVTKWidget;
}

void GridDataDialog::on_cbxConfiguration_currentIndexChanged(const QString &configurationName) {
    ui->tblGridLayers->setRowCount(0);
    
    if (configurationName.isEmpty()) {
        toggleGridDataConfigurationForm(false);
        ui->edtConfigurationName->clear();
        ui->cbxMesh->setCurrentIndex(-1);
        currentConfiguration = unsavedConfiguration;
        ui->btnSaveConfiguration->setText("Save");
    } else {
        Project *project = IPHApplication::getCurrentProject();
        currentConfiguration = project->getGridDataConfiguration(configurationName);
        QVector<GridData*> gridDataVector = currentConfiguration->getGridDataVector();

        ui->edtConfigurationName->setText(currentConfiguration->getName());
        ui->cbxMesh->setCurrentText(currentConfiguration->getMesh()->getName());

        for (int i = 0; i < gridDataVector.count(); i++) {
            int rowCount = ui->tblGridLayers->rowCount();
            GridData *gridData = gridDataVector.at(i);

            ui->tblGridLayers->insertRow(rowCount);
            ui->tblGridLayers->setItem(rowCount, 0, new QTableWidgetItem(gridData->getName()));
            ui->tblGridLayers->setItem(rowCount, 1, new QTableWidgetItem(gridData->gridDataTypeToString()));
            ui->tblGridLayers->setItem(rowCount, 2, new QTableWidgetItem(QString::number(gridData->getInputPolyData()->GetNumberOfPoints())));
        }

        ui->btnShowGridDataPoints->setEnabled(true);
        ui->btnShowGridDataPoints->toggled(false);
        ui->btnShowColorMap->setEnabled(true);
        ui->btnShowColorMap->toggled(true);
        toggleGridDataConfigurationForm(true);
    }
}

void GridDataDialog::on_cbxMesh_currentIndexChanged(const QString &meshName) {
    bool isMeshNamePresent = !meshName.isEmpty();

    if (isMeshNamePresent) {
        Mesh *configurationMesh = currentConfiguration->getMesh();
        
        if (currentMesh != nullptr && configurationMesh != nullptr && currentConfiguration->getMesh()->getName() != meshName && currentConfiguration->getGridDataVector().size() > 0) {
            QString question = tr("Changing the mesh in this configuration will remove all created grid data. Are you sure?");
            QMessageBox::StandardButton button = QMessageBox::question(this, tr("Grid Data"), question);
            
            if (button == QMessageBox::No) {
                ui->cbxMesh->blockSignals(true);
                ui->cbxMesh->setCurrentText(currentMesh->getName());
                ui->cbxMesh->blockSignals(false);
                return;
            }
            
            currentConfiguration->clearGridDataVector();
            ui->tblGridLayers->setRowCount(0);
        }
        
        currentMesh = IPHApplication::getCurrentProject()->getMesh(meshName);

        if (currentMesh->instanceOf("UnstructuredMesh")) {
            currentMesh = static_cast<UnstructuredMesh*>(currentMesh);
        } else {
            currentMesh = static_cast<StructuredMesh*>(currentMesh);
        }
    } else {
        currentMesh = nullptr;
    }

    ui->gridDataVTKWidget->render(currentMesh);
    ui->gridDataVTKWidget->toggleCellPick(false);
    ui->btnPickIndividualCells->setChecked(false);
    ui->btnPickCellSet->setChecked(false);
    ui->btnPickIndividualCells->setEnabled(isMeshNamePresent);
    ui->btnPickCellSet->setEnabled(isMeshNamePresent);
    ui->btnShowCellLabels->setEnabled(isMeshNamePresent);
    ui->btnShowCellWeights->setEnabled(isMeshNamePresent);
    ui->btnExport->setEnabled(isMeshNamePresent);
    ui->btnAddGridLayer->setEnabled(isMeshNamePresent);
    ui->btnRemoveGridLayer->setEnabled(isMeshNamePresent);
    ui->btnShowMesh->setEnabled(isMeshNamePresent);
}

void GridDataDialog::on_btnAddGridLayer_clicked() {
    showGridLayerDialog(nullptr);
}

void GridDataDialog::showGridLayerDialog(GridData *gridData) {
    bool isNewGridData = gridData == nullptr;
    GridLayerDialog *gridLayerDialog = new GridLayerDialog(this, currentConfiguration, gridData, currentMesh);
    int exitCode = gridLayerDialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        GridData *gridData = gridLayerDialog->getGridData();
        
        if (isNewGridData) {
            int maximum = gridData->getMaximumProgress();
            
            QProgressDialog *progressDialog = new QProgressDialog(this);
            progressDialog->setMinimum(0);
            progressDialog->setMaximum(maximum);
            progressDialog->setMinimumDuration(500);
            progressDialog->setWindowModality(Qt::WindowModal);

            QObject::connect(gridData, SIGNAL(updateProgressText(const QString&)), progressDialog, SLOT(setLabelText(const QString&)));
            QObject::connect(gridData, SIGNAL(updateProgress(int)), progressDialog, SLOT(setValue(int)));
            QObject::connect(progressDialog, SIGNAL(canceled()), gridData, SLOT(cancelInterpolation()));
            
            try {
                gridData->interpolate();
            } catch (const GridDataException &e) {
                QMessageBox::critical(this, tr("Grid Data"), e.what());
                progressDialog->cancel();
                delete progressDialog;
                delete gridData;
                return;
            }
            
            if (progressDialog->wasCanceled()) {
                gridData->cancelInterpolation(false); // Set false for future computations
            } else {
                int rowCount = ui->tblGridLayers->rowCount();
                
                currentConfiguration->addGridData(gridData);
                
                ui->cbxConfiguration->setEnabled(true);
                ui->tblGridLayers->insertRow(rowCount);
                ui->tblGridLayers->setItem(rowCount, 0, new QTableWidgetItem(gridData->getName()));
                ui->tblGridLayers->setItem(rowCount, 1, new QTableWidgetItem(gridData->gridDataTypeToString()));
                ui->tblGridLayers->setItem(rowCount, 2, new QTableWidgetItem(QString::number(gridData->getInputPolyData()->GetNumberOfPoints())));
                ui->tblGridLayers->setCurrentCell(rowCount, 0);
            }
            
            delete progressDialog;
        } else {
            int currentRow = ui->tblGridLayers->currentRow();
            QTableWidgetItem *nameItem = ui->tblGridLayers->item(currentRow, 0);
            QTableWidgetItem *griDataTypeItem = ui->tblGridLayers->item(currentRow, 1);
            
            nameItem->setText(gridData->getName());
            griDataTypeItem->setText(gridData->gridDataTypeToString());
        }
    }
}

void GridDataDialog::on_btnEditGridLayer_clicked() {
    int currentRow = ui->tblGridLayers->currentRow();

    if (currentRow != -1) {
        GridData *gridData = currentConfiguration->getGridData(ui->tblGridLayers->item(currentRow, 0)->text());
        showGridLayerDialog(gridData);
    }
}

void GridDataDialog::on_tblGridLayers_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    if (current != nullptr && (previous == nullptr || current->row() != previous->row())) {
        QString gridDataName = ui->tblGridLayers->item(current->row(), 0)->text();
        GridData *gridData = currentConfiguration->getGridData(gridDataName);
        
        ui->gridDataVTKWidget->toggleMapPoints(ui->btnShowGridDataPoints->isChecked());
        ui->gridDataVTKWidget->toggleMap(ui->btnShowColorMap->isChecked());
        ui->gridDataVTKWidget->render(gridData);
        ui->btnEditGridLayer->setEnabled(true);
    }
}

void GridDataDialog::on_tblGridLayers_cellDoubleClicked(int row, int column) {
    QString gridDataName = ui->tblGridLayers->item(row, 0)->text();
    GridData *gridData = currentConfiguration->getGridData(gridDataName);
    GridLayerAttributesDialog *dialog = new GridLayerAttributesDialog(this, gridData);
    
    dialog->exec();
}

void GridDataDialog::on_btnRemoveGridLayer_clicked() {
    int currentRow = ui->tblGridLayers->currentRow();

    if (currentRow > -1 && QMessageBox::question(this, tr("Grid Data"), tr("Are you sure you want to remove the selected grid layer?")) == QMessageBox::Yes) {
        currentConfiguration->removeGridData(currentRow);
        ui->gridDataVTKWidget->clear();
        ui->tblGridLayers->removeRow(currentRow);
        
        if (ui->tblGridLayers->rowCount() == 0) {
            if (currentConfiguration->isPersisted()) {
                ui->cbxConfiguration->setEnabled(false);
            }
            
            toggleGridDataConfigurationForm(false);
            ui->btnEditGridLayer->setEnabled(false);
            ui->btnShowGridDataPoints->setEnabled(false);
            ui->btnShowGridDataPoints->toggled(false);
            ui->btnShowColorMap->setEnabled(false);
            ui->btnShowColorMap->toggled(false);
        }
    }
}

void GridDataDialog::toggleGridDataConfigurationForm(bool enable) {
    ui->btnDoneConfiguration->setEnabled(enable);
    ui->btnRemoveConfiguration->setEnabled(enable);
}

void GridDataDialog::on_btnSaveConfiguration_clicked() {
    QString configurationName = ui->edtConfigurationName->text();
    
    if (!isConfigurationValid(configurationName)) {
        return;
    }
    
    currentConfiguration->setName(configurationName);

    if (ui->cbxConfiguration->currentIndex() == -1) {
        IPHApplication::getCurrentProject()->addGridDataConfiguration(currentConfiguration);
        unsavedConfiguration = new GridDataConfiguration();

        ui->cbxConfiguration->addItem(configurationName);
        ui->cbxConfiguration->setCurrentText(configurationName);
    } else {
        ui->cbxConfiguration->setItemText(ui->cbxConfiguration->currentIndex(), configurationName);
        toggleGridDataConfigurationForm(true);
    }
}

void GridDataDialog::on_btnRemoveConfiguration_clicked() {
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("Grid Data"), tr("Are you sure you want to remove this configuration?"));

    if (button == QMessageBox::Yes) {
        IPHApplication::getCurrentProject()->removeGridDataConfiguration(ui->cbxConfiguration->currentText());

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
    
    if (ui->cbxConfiguration->currentText() != configurationName && ui->cbxConfiguration->findText(configurationName, Qt::MatchFixedString) != -1) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Configuration name already used."));
        return false;
    }

    if (ui->cbxMesh->currentIndex() == -1) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Associate a mesh to this configuration."));
        return false;
    }

    if (ui->tblGridLayers->rowCount() == 0) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Please input at least one grid information."));
        return false;
    }

    return true;
}

void GridDataDialog::on_btnBackgroundColor_clicked() {
    QColor color = QColorDialog::getColor(Qt::white, this, "Select a background color");
    
    if (color.isValid()) {
        QPixmap px(24, 24);
        px.fill(color);
        
        ui->gridDataVTKWidget->changeBackgroundColor(color.redF(), color.greenF(), color.blueF());
        ui->btnBackgroundColor->setIcon(px);
    }
}

void GridDataDialog::on_btnPickIndividualCells_clicked(bool checked) {
    ui->btnShowCellLabels->setChecked(false);
    ui->btnShowCellLabels->setEnabled(!checked);
    ui->btnShowCellWeights->setChecked(false);
    ui->btnShowCellWeights->setEnabled(!checked);
    ui->btnPickCellSet->setChecked(false);
    ui->btnPickCellSet->setEnabled(!checked);
    ui->gridDataVTKWidget->toggleCellLabels(CellLabelType::UNDEFINED);
    ui->gridDataVTKWidget->toggleCellPick(checked, CellPickMode::INDIVIDUAL);
}

void GridDataDialog::on_btnPickCellSet_clicked(bool checked) {
    ui->btnShowCellLabels->setChecked(false);
    ui->btnShowCellLabels->setEnabled(!checked);
    ui->btnShowCellWeights->setChecked(false);
    ui->btnShowCellWeights->setEnabled(!checked);
    ui->btnPickIndividualCells->setChecked(false);
    ui->btnPickIndividualCells->setEnabled(!checked);
    ui->gridDataVTKWidget->toggleCellLabels(CellLabelType::UNDEFINED);
    ui->gridDataVTKWidget->toggleCellPick(checked, CellPickMode::MULTIPLE);
}

void GridDataDialog::on_btnShowCellLabels_clicked(bool checked) {
    ui->btnShowCellWeights->setChecked(false);
    ui->btnShowCellWeights->setEnabled(!checked);
    ui->gridDataVTKWidget->toggleCellPick(false);
    ui->gridDataVTKWidget->toggleCellLabels(checked ? CellLabelType::ID : CellLabelType::UNDEFINED);
}

void GridDataDialog::on_btnShowCellWeights_clicked(bool checked) {
    ui->btnShowCellLabels->setChecked(false);
    ui->btnShowCellLabels->setEnabled(!checked);
    ui->gridDataVTKWidget->toggleCellPick(false);
    ui->gridDataVTKWidget->toggleCellLabels(checked ? CellLabelType::WEIGHT : CellLabelType::UNDEFINED);
}

void GridDataDialog::on_btnLockView_clicked(bool checked) {
    ui->btnNavigateMode->setEnabled(!checked);
    ui->btnOriginalZoom->setEnabled(!checked);
    ui->btnZoomArea->setEnabled(!checked);
    ui->btnShowAxes->setEnabled(!checked);
    ui->btnPickIndividualCells->setEnabled(!checked);
    ui->btnPickCellSet->setEnabled(!checked);
    ui->btnShowGridDataPoints->setEnabled(!checked);
    ui->btnShowColorMap->setEnabled(!checked);
    ui->btnShowMesh->setEnabled(!checked);
    ui->btnShowCellLabels->setEnabled(!checked);
    ui->btnShowCellWeights->setEnabled(!checked);
    ui->btnBackgroundColor->setEnabled(!checked);
    ui->gridDataVTKWidget->lockView(checked);
}

QString GridDataDialog::getDefaultDirectory() {
    return appSettings->value(GRID_DATA_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(GRID_DATA_DEFAULT_DIR_KEY).toString();
}

void GridDataDialog::on_btnExport_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export to PNG"), getDefaultDirectory(), tr("Portable Network Graphics (*.png)"));
    
    if (!fileName.isEmpty()) {
        ui->gridDataVTKWidget->exportToImage(fileName);
        QMessageBox::information(this, tr("Grid Data"), tr("Map exported."));
    }
}