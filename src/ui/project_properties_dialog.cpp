#include "include/ui/project_properties_dialog.h"
#include "ui_project_properties_dialog.h"

#include "include/application/iph_application.h"
#include "include/services/project_service.h"

#include <QDialogButtonBox>
#include <QMessageBox>

ProjectPropertiesDialog::ProjectPropertiesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ProjectPropertiesDialog) {
    Project *project = IPHApplication::getCurrentProject();
    
    ui->setupUi(this);
    ui->edtName->setText(project->getName());
    ui->txtDescription->setPlainText(project->getDescription());
    ui->cbxHydrodynamic->setChecked(project->getHydrodynamic());
    ui->cbxSediment->setChecked(project->getSediment());
    ui->cbxWaterQuality->setChecked(project->getWaterQuality());
}

ProjectPropertiesDialog::~ProjectPropertiesDialog() {
    delete ui;
}

void ProjectPropertiesDialog::accept() {
    if (!isFormValid()) {
        return;
    }
    
    Project *project = IPHApplication::getCurrentProject();
    
    project->setName(ui->edtName->text());
    project->setDescription(ui->txtDescription->toPlainText());
    project->setHydrodynamic(ui->cbxHydrodynamic->isChecked());
    project->setSediment(ui->cbxSediment->isChecked());
    project->setWaterQuality(ui->cbxWaterQuality->isChecked());
    
    QDialog::accept();
}

bool ProjectPropertiesDialog::isFormValid() {
    if (ui->edtName->text().isEmpty()) {
        QMessageBox::warning(this, tr("New Project"), tr("Project name can't be blank."));
        return false;
    }

    if (!ui->cbxHydrodynamic->isChecked() && !ui->cbxWaterQuality->isChecked() && !ui->cbxSediment->isChecked()) {
        QMessageBox::warning(this, tr("New Project"), tr("At least one analysis type must be checked"));
        return false;
    }

    return true;
}
