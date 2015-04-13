#include "include/ui/grid_data_dialog.h"
#include "ui_grid_data_dialog.h"

#include <QFileDialog>

#include "include/application/iph_application.h"
#include "include/domain/structured_mesh.h"
#include "include/domain/unstructured_mesh.h"

GridDataDialog::GridDataDialog(QWidget *parent) :
    QDialog(parent), GRID_DATA_DEFAULT_DIR_KEY("grid_data_default_dir"), ui(new Ui::GridDataDialog), unsavedGridData(new GridData()), currentGridData(unsavedGridData) {
    ui->setupUi(this);

    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);
    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();

    for (QSet<Mesh*>::iterator i = meshes.begin(); i != meshes.end(); ++i) {
        ui->cbxMesh->addItem((*i)->getName());
    }
    ui->cbxMesh->setCurrentIndex(-1);
}

GridDataDialog::~GridDataDialog() {
    delete unsavedGridData;
    delete ui;
}

QString GridDataDialog::getDefaultDirectory() {
    return appSettings->value(GRID_DATA_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(GRID_DATA_DEFAULT_DIR_KEY).toString();
}

void GridDataDialog::on_btnBathymetry_clicked() {
    QString bathymetryFile = QFileDialog::getOpenFileName(this, tr("Select a XY file"), getDefaultDirectory(), "XY file (*.xy)");

    if (bathymetryFile.isEmpty()) {
        return;
    }

    ui->edtBathymetryFile->setText(bathymetryFile);
}

void GridDataDialog::on_btnWindReduction_clicked() {
    QString windReductionFile = QFileDialog::getOpenFileName(this, tr("Select a XY file"), getDefaultDirectory(), "XY file (*.xy)");

    if (windReductionFile.isEmpty()) {
        return;
    }

    ui->edtWindReductionFile->setText(windReductionFile);
}

void GridDataDialog::on_btnChezy_clicked() {
    QString chezyCoefficientFile = QFileDialog::getOpenFileName(this, tr("Select a XY file"), getDefaultDirectory(), "XY file (*.xy)");

    if (chezyCoefficientFile.isEmpty()) {
        return;
    }

    ui->edtChezyFile->setText(chezyCoefficientFile);
}

void GridDataDialog::on_btnWetlandAreas_clicked() {
    QString wetlandAreasFile = QFileDialog::getOpenFileName(this, tr("Select a XY file"), getDefaultDirectory(), "XY file (*.xy)");

    if (wetlandAreasFile.isEmpty()) {
        return;
    }

    ui->edtWetlandAreasFile->setText(wetlandAreasFile);
}

void GridDataDialog::on_cbxMesh_currentIndexChanged(const QString &currentItem) {
    Project *project = IPHApplication::getCurrentProject();
    Mesh *mesh = project->getMesh(currentItem);

    if (dynamic_cast<UnstructuredMesh*>(mesh) != NULL) {
        mesh = static_cast<UnstructuredMesh*>(mesh);
    } else {
        mesh = static_cast<StructuredMesh*>(mesh);
    }

    ui->gridDataOpenGLWidget->setMesh(mesh);
}
