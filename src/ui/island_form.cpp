#include <ui/island_form.h>
#include "ui_island_form.h"
#include <exceptions/mesh_polygon_exception.h>
#include <ui/coordinate_file_dialog.h>

#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>

IslandForm::IslandForm(QDialog *parent, Mesh *mesh) :
    QDialog(parent), ISLAND_FORM_DEFAULT_DIR_KEY("island_form_default_dir"), ui(new Ui::IslandForm), mesh(mesh), meshPolygon(nullptr)
{
    ui->setupUi(this);
    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);
}

IslandForm::~IslandForm() {
    delete ui;
}

QString IslandForm::getDefaultDirectory() {
    return appSettings->value(ISLAND_FORM_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(ISLAND_FORM_DEFAULT_DIR_KEY).toString();
}

bool IslandForm::isValid() {
    QString meshPolygonName = ui->edtName->text();
    
    if (meshPolygonName.isEmpty()) {
        QMessageBox::warning(this, tr("Mesh Island"), tr("Name can't be blank."));
        return false;
    }
    
    if (mesh->getMeshPolygon(meshPolygonName, MeshPolygonType::ISLAND)) {
        QMessageBox::warning(this, tr("Mesh Island"), tr("Name already used."));
        return false;
    }
    
    if (ui->edtFile->text().isEmpty()) {
        QMessageBox::warning(this, tr("Mesh Island"), tr("File can't be blank."));
        return false;
    }
    
    return true;
}

void IslandForm::accept() {
    if (!isValid()) {
        return;
    }
    
    try {
        meshPolygon = mesh->addMeshPolygon(ui->edtName->text(), ui->edtFile->text(), MeshPolygonType::ISLAND, this->coordinateSystem);
        QDialog::accept();
    } catch (const MeshPolygonException &e) {
        QMessageBox::warning(this, tr("Mesh Island"), e.what());
    }
}

void IslandForm::on_btnBrowse_clicked() {
    CoordinateFileDialog::CoordinateFileDialogFilter filters = static_cast<CoordinateFileDialog::CoordinateFileDialogFilter>(CoordinateFileDialog::FILTER_TEXT | CoordinateFileDialog::FILTER_KML);
    CoordinateFileDialog *dialog = new CoordinateFileDialog(this, tr("Select a island file"), getDefaultDirectory(), filters);
    int exitCode = dialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        QString islandFile = dialog->selectedFiles().first();
        
        this->coordinateSystem = dialog->isLatitudeLongitudeChecked() ? CoordinateSystem::LATITUDE_LONGITUDE : CoordinateSystem::UTM;
        ui->edtFile->setText(islandFile);
        appSettings->setValue(ISLAND_FORM_DEFAULT_DIR_KEY, QFileInfo(islandFile).absolutePath());
    }
}

MeshPolygon* IslandForm::getMeshPolygon() const {
    return meshPolygon;
}