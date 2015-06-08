#include "include/ui/grid_data_dialog.h"
#include "ui_grid_data_dialog.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QVector>
#include <QCheckBox>
#include <QObject>
#include <QHBoxLayout>
#include <QDebug>

#include "include/application/iph_application.h"
#include "include/domain/structured_mesh.h"
#include "include/domain/unstructured_mesh.h"
#include "include/exceptions/grid_data_exception.h"

GridDataDialog::GridDataDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::GridDataDialog),
    unsavedGridDataConfiguration(new GridDataConfiguration()), currentGridDataConfiguration(unsavedGridDataConfiguration)
{
    ui->setupUi(this);
    ui->tblGridInformation->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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

    ui->gridDataOpenGLWidget->setGridDataConfiguration(currentGridDataConfiguration);
}

GridDataDialog::~GridDataDialog() {
    delete unsavedGridDataConfiguration;
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

    if (configurationName.isEmpty()) {
        currentGridDataConfiguration = unsavedGridDataConfiguration;
        toggleGridDataConfigurationForm(false);
        ui->edtConfigurationName->clear();
        ui->cbxMesh->setCurrentIndex(-1);
    } else {
        Project *project = IPHApplication::getCurrentProject();
        currentGridDataConfiguration = project->getGridDataConfiguration(configurationName);
        QVector<GridData*> &gridDataVector = currentGridDataConfiguration->getGridDataVector();

        ui->edtConfigurationName->setText(currentGridDataConfiguration->getName());
        ui->cbxMesh->setCurrentText(currentGridDataConfiguration->getMesh()->getName());

        for (int i = 0; i < gridDataVector.count(); i++) {
            int rowCount = ui->tblGridInformation->rowCount();
            GridData *gridData = gridDataVector.at(i);
            QWidget *checkBoxWidget = createCheckBoxWidget(gridData);

            ui->tblGridInformation->insertRow(rowCount);
            ui->tblGridInformation->setItem(rowCount, 0, new QTableWidgetItem(gridData->gridDataInputTypeToString()));
            ui->tblGridInformation->setItem(rowCount, 1, new QTableWidgetItem(gridData->getGridDataType().toString()));
            ui->tblGridInformation->setCellWidget(rowCount, 2, checkBoxWidget);
        }

        toggleGridDataConfigurationForm(true);
    }

    ui->gridDataOpenGLWidget->setGridDataConfiguration(currentGridDataConfiguration);
}

void GridDataDialog::on_cbxMesh_currentIndexChanged(const QString &meshName) {
    Mesh *mesh = NULL;
    bool isMeshNamePresent = !meshName.isEmpty();

    if (isMeshNamePresent) {
        Project *project = IPHApplication::getCurrentProject();
        mesh = project->getMesh(meshName);

        if (dynamic_cast<UnstructuredMesh*>(mesh) != NULL) {
            mesh = static_cast<UnstructuredMesh*>(mesh);
        } else {
            mesh = static_cast<StructuredMesh*>(mesh);
        }
    }

    currentGridDataConfiguration->setMesh(mesh);
    ui->gridDataOpenGLWidget->update();
    ui->btnAddGridInfomation->setEnabled(isMeshNamePresent);
    ui->btnRemoveGridInformation->setEnabled(isMeshNamePresent);
}

void GridDataDialog::on_btnAddGridInfomation_clicked() {
    GridInformationDialog *gridInformationDialog = new GridInformationDialog(this, NULL);
    int exitCode = gridInformationDialog->exec();

    if (exitCode == QDialog::Accepted) {
        int rowCount = ui->tblGridInformation->rowCount();
        GridData *gridData = gridInformationDialog->getGridData();
        QWidget *checkBoxWidget = createCheckBoxWidget(gridData);

        currentGridDataConfiguration->addGridData(gridData);

        ui->tblGridInformation->insertRow(rowCount);
        ui->tblGridInformation->setItem(rowCount, 0, new QTableWidgetItem(gridData->gridDataInputTypeToString()));
        ui->tblGridInformation->setItem(rowCount, 1, new QTableWidgetItem(gridData->getGridDataType().toString()));
        ui->tblGridInformation->setCellWidget(rowCount, 2, checkBoxWidget);
    }
}

void GridDataDialog::on_tblGridInformation_itemDoubleClicked(QTableWidgetItem *item) {
    GridData *gridData = currentGridDataConfiguration->getGridData(item->row());
    GridInformationDialog *gridInformationDialog = new GridInformationDialog(this, gridData);
    int exitCode = gridInformationDialog->exec();

    if (exitCode == QDialog::Accepted) {
        QTableWidgetItem *inputTypeItem = ui->tblGridInformation->item(item->row(), 0);
        QTableWidgetItem *gridInformationItem = ui->tblGridInformation->item(item->row(), 1);

        inputTypeItem->setText(gridData->gridDataInputTypeToString());
        gridInformationItem->setText(gridData->getGridDataType().toString());
    }
}

void GridDataDialog::on_btnRemoveGridInformation_clicked() {
    int currentRow = ui->tblGridInformation->currentRow();

    if (currentRow > -1 && QMessageBox::Yes == QMessageBox::question(this, tr("Grid Data"), tr("Are you sure?"))) {
        currentGridDataConfiguration->removeGridData(currentRow);
        ui->tblGridInformation->removeRow(currentRow);
        ui->gridDataOpenGLWidget->update();
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

    checkBox->setChecked(gridData->getShow());
    QObject::connect(checkBox, SIGNAL(clicked(bool)), gridData, SLOT(setShow(bool)));

    return widget;
}

void GridDataDialog::toggleGridDataConfigurationForm(bool enable) {
    ui->btnDoneConfiguration->setEnabled(enable);
    ui->btnRemoveConfiguration->setEnabled(enable);
    ui->btnSaveAsNewConfiguration->setEnabled(enable);
}

void GridDataDialog::on_btnSaveConfiguration_clicked() {
    if (!isConfigurationValid()) {
        return;
    }

    Project *project = IPHApplication::getCurrentProject();
    QString meshName = ui->cbxMesh->currentText();
    Mesh *mesh = project->getMesh(meshName);
    QString configurationName = ui->cbxConfiguration->currentIndex() == -1 ? ui->edtConfigurationName->text() : ui->cbxConfiguration->currentText();
    GridDataConfiguration *gridDataConfiguration = project->getGridDataConfiguration(configurationName);

    if (gridDataConfiguration == NULL && ui->cbxConfiguration->currentIndex() == -1) {
        currentGridDataConfiguration->setName(configurationName);
        currentGridDataConfiguration->setMesh(mesh);
        project->addGridDataConfiguration(currentGridDataConfiguration);

        unsavedGridDataConfiguration = new GridDataConfiguration();
        currentGridDataConfiguration = unsavedGridDataConfiguration;

        ui->cbxConfiguration->addItem(configurationName);
        ui->cbxConfiguration->setCurrentText(configurationName);
    } else {
        configurationName = ui->edtConfigurationName->text();

        if (configurationName.isEmpty()) {
            QMessageBox::warning(this, tr("Grid Data"), tr("Configuration name can't be empty."));
            return;
        }

        currentGridDataConfiguration->setName(configurationName);
        currentGridDataConfiguration->setMesh(mesh);
        ui->cbxConfiguration->setItemText(ui->cbxConfiguration->currentIndex(), configurationName);
    }
}

void GridDataDialog::on_btnSaveAsNewConfiguration_clicked() {
    QString newConfigurationName = QInputDialog::getText(this, tr("Save Grid Data Configuration as New"), tr("New Configuration Name"));

    if (!newConfigurationName.isEmpty() && isConfigurationValid()) {
        GridDataConfiguration *newConfiguration = new GridDataConfiguration();
        Project *project = IPHApplication::getCurrentProject();
        QString meshName = ui->cbxMesh->currentText();
        Mesh *mesh = project->getMesh(meshName);
        QVector<GridData*> gridDataVector = currentGridDataConfiguration->getGridDataVector();

        newConfiguration->setName(newConfigurationName);
        newConfiguration->setMesh(mesh);

        for (int i = 0; i < gridDataVector.count(); i++) {
            GridData *gridData = gridDataVector.at(i);
            GridData *newGridData = new GridData();

            newGridData->setGridDataInputType(gridData->getGridDataInputType());
            newGridData->setInputFile(gridData->getInputFile());
            newGridData->setGridDataType(gridData->getGridDataType());
            newGridData->setExponent(gridData->getExponent());
            newGridData->setRadius(gridData->getRadius());
            newGridData->setShow(gridData->getShow());

            newConfiguration->addGridData(newGridData);
        }

        currentGridDataConfiguration = newConfiguration;

        ui->cbxConfiguration->addItem(newConfigurationName);
        ui->cbxConfiguration->setCurrentText(newConfigurationName);
    }
}

void GridDataDialog::on_btnRemoveConfiguration_clicked() {
    QString configurationName = ui->cbxConfiguration->currentText();

    if (QMessageBox::Yes == QMessageBox::question(this, tr("Remove grid data"), tr("Are you sure you want to remove '") + configurationName + "'?")) {
        IPHApplication::getCurrentProject()->removeGridDataConfiguration(configurationName);

        ui->cbxConfiguration->removeItem(ui->cbxConfiguration->currentIndex());
        ui->cbxConfiguration->setCurrentIndex(-1);
        ui->gridDataOpenGLWidget->setGridDataConfiguration(NULL);
    }
}

void GridDataDialog::on_btnDoneConfiguration_clicked() {
    ui->cbxConfiguration->setCurrentIndex(-1);
}

bool GridDataDialog::isConfigurationValid() {
    if (ui->edtConfigurationName->text().isEmpty()) {
        QMessageBox::warning(this, tr("Grid Data"), tr("Please input the configuration name."));
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

void GridDataDialog::on_chkShowMesh_toggled(bool checked) {
    currentGridDataConfiguration->setShowMesh(checked);
    ui->gridDataOpenGLWidget->update();
}
