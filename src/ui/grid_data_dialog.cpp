#include "include/ui/grid_data_dialog.h"
#include "ui_grid_data_dialog.h"

#include "include/application/iph_application.h"
#include "include/domain/structured_mesh.h"
#include "include/domain/unstructured_mesh.h"
#include "include/exceptions/grid_data_exception.h"
#include "include/ui/main_window.h"
#include "include/ui/grid_layer_dialog.h"
#include "include/ui/grid_layer_attributes_dialog.h"

#include <QProgressDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QVector>
#include <QObject>

GridDataDialog::GridDataDialog(QWidget *parent) :
    AbstractMeshDialog(parent), GRID_DATA_DEFAULT_DIR_KEY("grid_data_default_dir"), ui(new Ui::GridDataDialog),
    unsavedConfiguration(new GridDataConfiguration), currentConfiguration(unsavedConfiguration), currentMesh(nullptr)
{
    ui->setupUi(this);
    this->vtkWidget = ui->vtkWidget;
    
    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);

	Qt::WindowFlags flags = this->windowFlags() | Qt::WindowMaximizeButtonHint;
	this->setWindowFlags(flags);
    ui->tblGridLayers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();
    QSet<GridDataConfiguration*> configurations = project->getGridDataConfigurations();

    ui->cbxMesh->blockSignals(true);
    for (Mesh *mesh : meshes) {
        ui->cbxMesh->addItem(mesh->getName());
    }
    ui->cbxMesh->setCurrentIndex(-1);
    ui->cbxMesh->blockSignals(false);

    ui->cbxConfiguration->blockSignals(true);
    for (GridDataConfiguration *configuration : configurations) {
        ui->cbxConfiguration->addItem(configuration->getName());
    }
    ui->cbxConfiguration->setCurrentIndex(-1);
    ui->cbxConfiguration->blockSignals(false);
    
    ui->vtkWidget->clear();
}

GridDataDialog::~GridDataDialog() {
    delete unsavedConfiguration;
    delete ui;
}

GridDataVTKWidget* GridDataDialog::getVTKWidget() {
    return ui->vtkWidget;
}

void GridDataDialog::on_cbxConfiguration_currentIndexChanged(const QString &configurationName) {
    ui->tblGridLayers->setRowCount(0);
    
    if (configurationName.isEmpty()) {
        return;
    }
    

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
        ui->tblGridLayers->setItem(rowCount, 1, new QTableWidgetItem(GridData::getGridTypesMap().value(gridData->getGridDataType())));
        ui->tblGridLayers->setItem(rowCount, 2, new QTableWidgetItem(QString::number(gridData->getInputPolyData()->GetNumberOfPoints())));
    }

    showGridDataPointsAction->setEnabled(true);
    showGridDataPointsAction->toggled(false);
    showColorMapAction->setEnabled(true);
    showColorMapAction->toggled(true);
}

void GridDataDialog::on_cbxMesh_currentIndexChanged(const QString &meshName) {
    if (meshName.isEmpty()) {
        ui->vtkWidget->clear();
        return;
    }
    
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

    ui->vtkWidget->render(currentMesh);
    ui->vtkWidget->toggleCellPick(false);
    pickIndividualCellAction->setChecked(false);
    pickCellSetAction->setChecked(false);
    pickIndividualCellAction->setEnabled(true);
    pickCellSetAction->setEnabled(true);
    toggleCellLabelsAction->setEnabled(true);
    showCellWeightsAction->setEnabled(true);
    ui->btnAddGridLayer->setEnabled(true);
    ui->btnRemoveGridLayer->setEnabled(true);
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
                progressDialog->cancel();
                QMessageBox::critical(this, tr("Grid Data"), e.what());
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
                ui->tblGridLayers->setItem(rowCount, 1, new QTableWidgetItem(GridData::getGridTypesMap().value(gridData->getGridDataType())));
                ui->tblGridLayers->setItem(rowCount, 2, new QTableWidgetItem(QString::number(gridData->getInputPolyData()->GetNumberOfPoints())));
                ui->tblGridLayers->setCurrentCell(rowCount, 0);
            }
            
            delete progressDialog;
        } else {
            int currentRow = ui->tblGridLayers->currentRow();
            QTableWidgetItem *nameItem = ui->tblGridLayers->item(currentRow, 0);
            QTableWidgetItem *griDataTypeItem = ui->tblGridLayers->item(currentRow, 1);
            
            nameItem->setText(gridData->getName());
            griDataTypeItem->setText(GridData::getGridTypesMap().value(gridData->getGridDataType()));
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
        
        ui->vtkWidget->toggleMapPoints(showGridDataPointsAction->isChecked());
        ui->vtkWidget->toggleMap(showColorMapAction->isChecked());
        ui->vtkWidget->render(gridData);
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
        ui->vtkWidget->clear();
        ui->tblGridLayers->removeRow(currentRow);
        
        if (ui->tblGridLayers->rowCount() == 0) {
            if (currentConfiguration->isPersisted()) {
                ui->cbxConfiguration->setEnabled(false);
            }
            
            ui->btnEditGridLayer->setEnabled(false);
            showGridDataPointsAction->setEnabled(false);
            showGridDataPointsAction->toggled(false);
            showColorMapAction->setEnabled(false);
            showColorMapAction->toggled(false);
        }
    }
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
    
    if (currentConfiguration->getGridData(GridDataType::BATHYMETRY).isEmpty()) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Bathymetry layer is required."));
        return false;
    }
    
    if (currentConfiguration->getGridData(GridDataType::ROUGHNESS).isEmpty()) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Roughness layer is required."));
        return false;
    }

    return true;
}

void GridDataDialog::onPickIndividualCellAction(bool checked) {
    toggleCellLabelsAction->setChecked(false);
    toggleCellLabelsAction->setEnabled(!checked);
    toggleVerticeLabelsAction->setChecked(false);
    toggleVerticeLabelsAction->setEnabled(!checked);
    zoomAreaAction->setChecked(false);
    zoomAreaAction->setEnabled(!checked);
    showCellWeightsAction->setChecked(false);
    showCellWeightsAction->setEnabled(!checked);
    pickCellSetAction->setChecked(false);
    pickCellSetAction->setEnabled(!checked);
    ui->vtkWidget->toggleCellLabels(LabelType::UNDEFINED);
    ui->vtkWidget->toggleCellPick(checked, PickerMode::INDIVIDUAL_CELL);
}

void GridDataDialog::onPickCellSetAction(bool checked) {
    toggleCellLabelsAction->setChecked(false);
    toggleCellLabelsAction->setEnabled(!checked);
    zoomAreaAction->setChecked(false);
    zoomAreaAction->setEnabled(!checked);
    showCellWeightsAction->setChecked(false);
    showCellWeightsAction->setEnabled(!checked);
    pickIndividualCellAction->setChecked(false);
    pickIndividualCellAction->setEnabled(!checked);
    ui->vtkWidget->toggleZoomArea(!checked);
    ui->vtkWidget->toggleCellLabels(LabelType::UNDEFINED);
    ui->vtkWidget->toggleCellPick(checked, PickerMode::MULTIPLE_CELL);
}

void GridDataDialog::onShowCellWeightsAction(bool checked) {
    toggleCellLabelsAction->setChecked(false);
    toggleCellLabelsAction->setEnabled(!checked);
    ui->vtkWidget->toggleCellPick(false);
    ui->vtkWidget->toggleCellLabels(checked ? LabelType::CELL_WEIGHT : LabelType::UNDEFINED);
}

void GridDataDialog::onZoomAreaAction(bool checked) {
    pickCellSetAction->setChecked(false);
    pickCellSetAction->setEnabled(!checked);
    pickIndividualCellAction->setChecked(false);
    pickIndividualCellAction->setEnabled(!checked);
}

/*void GridDataDialog::on_btnLockView_clicked(bool checked) {
    ui->btnNavigateMode->setEnabled(!checked);
    ui->btnOriginalZoom->setEnabled(!checked);
    ui->btnZoomArea->setEnabled(!checked);
    ui->btnShowAxes->setEnabled(!checked);
    ui->btnPickIndividualCells->setEnabled(!checked);
    ui->btnPickCellSet->setEnabled(!checked);
    ui->btnShowGridDataPoints->setEnabled(!checked);
    ui->btnShowColorMap->setEnabled(!checked);
    ui->btnShowCellLabels->setEnabled(!checked);
    showCellWeightsAction->setEnabled(!checked);
    ui->btnBackgroundColor->setEnabled(!checked);
    ui->vtkWidget->lockView(checked);
}*/

QString GridDataDialog::getDefaultDirectory() {
    return appSettings->value(GRID_DATA_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(GRID_DATA_DEFAULT_DIR_KEY).toString();
}

void GridDataDialog::showEvent(QShowEvent *event) {
    if (!event->spontaneous() && this->windowState() & Qt::WindowMaximized) {
        MainWindow *mainWindow = static_cast<MainWindow*>(this->topLevelWidget());
        QAction *separator = mainWindow->getToolBar()->addSeparator();
        toolBarActions.append(separator);
        
        pickIndividualCellAction = new QAction(QIcon(":/icons/pick-individual-cell.png"), "Pick individual cells", mainWindow);
        pickIndividualCellAction->setCheckable(true);
        QObject::connect(pickIndividualCellAction, SIGNAL(triggered(bool)), this, SLOT(onPickIndividualCellAction(bool)));
        toolBarActions.append(pickIndividualCellAction);
        
        pickCellSetAction = new QAction(QIcon(":/icons/pick-cell-set.png"), "Pick cells from selected area", mainWindow);
        pickCellSetAction->setCheckable(true);
        QObject::connect(pickCellSetAction, SIGNAL(triggered(bool)), this, SLOT(onPickCellSetAction(bool)));
        toolBarActions.append(pickCellSetAction);
        
        showGridDataPointsAction = new QAction(QIcon(":/icons/show-hide-points.png"), "Show/Hide input points", mainWindow);
        showGridDataPointsAction->setCheckable(true);
        QObject::connect(showGridDataPointsAction, SIGNAL(triggered(bool)), vtkWidget, SLOT(toggleMapPoints(bool)));
        toolBarActions.append(showGridDataPointsAction);
        
        showColorMapAction = new QAction(QIcon(":/icons/interpolation-result.png"), "Show/Hide color map", mainWindow);
        showColorMapAction->setCheckable(true);
        showColorMapAction->setChecked(true);
        QObject::connect(showColorMapAction, SIGNAL(triggered(bool)), vtkWidget, SLOT(toggleMap(bool)));
        toolBarActions.append(showColorMapAction);
        
        showCellWeightsAction = new QAction(QIcon(":/icons/show-cell-weights.png"), "Show/Hide interpolated values", mainWindow);
        showCellWeightsAction->setCheckable(true);
        QObject::connect(showCellWeightsAction, SIGNAL(triggered(bool)), this,  SLOT(onShowCellWeightsAction(bool)));
        toolBarActions.append(showCellWeightsAction);
    }
    
    AbstractMeshDialog::showEvent(event);
    
    if (!event->spontaneous() && this->windowState() & Qt::WindowMaximized) {
        QObject::connect(zoomAreaAction, SIGNAL(triggered(bool)), this, SLOT(onZoomAreaAction(bool)));
    }
}

void GridDataDialog::on_btnNewConfiguration_clicked() {
    currentConfiguration = unsavedConfiguration;
    ui->cbxConfiguration->blockSignals(true);
    ui->cbxConfiguration->setCurrentIndex(-1);
    ui->cbxConfiguration->blockSignals(false);
    ui->cbxMesh->blockSignals(true);
    ui->cbxMesh->setCurrentIndex(-1);
    ui->cbxMesh->blockSignals(false);
    ui->edtConfigurationName->setText("");
    ui->edtConfigurationName->setFocus();
    ui->tblGridLayers->setRowCount(0);
    ui->vtkWidget->clear();
}

void GridDataDialog::on_btnApplyConfiguration_clicked() {
    QString oldConfigurationName = ui->cbxConfiguration->currentText();
    QString newConfigurationName = ui->edtConfigurationName->text();
    
    if (!isConfigurationValid(newConfigurationName)) {
        return;
    }
    
    currentConfiguration->setName(newConfigurationName);
    
    if (oldConfigurationName.isEmpty()) { // new configuration
        IPHApplication::getCurrentProject()->addGridDataConfiguration(currentConfiguration);
        unsavedConfiguration = new GridDataConfiguration();
        
        ui->cbxConfiguration->addItem(newConfigurationName);
        ui->cbxConfiguration->setCurrentText(newConfigurationName);
    } else {
        ui->cbxConfiguration->setItemText(ui->cbxConfiguration->currentIndex(), newConfigurationName);
    }
}

void GridDataDialog::on_btnRemoveConfiguration_clicked() {
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("Grid Data"), tr("Are you sure you want to remove this configuration?"));
    
    if (button == QMessageBox::Yes) {
        IPHApplication::getCurrentProject()->removeGridDataConfiguration(ui->cbxConfiguration->currentText());
        
        ui->cbxConfiguration->removeItem(ui->cbxConfiguration->currentIndex());
        this->on_btnNewConfiguration_clicked();
    }
}