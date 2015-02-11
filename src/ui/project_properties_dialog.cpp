#include "include/ui/project_properties_dialog.h"
#include "ui_project_properties_dialog.h"

#include <QDialogButtonBox>
#include <QMessageBox>

#include "include/services/project_service.h"

ProjectPropertiesDialog::ProjectPropertiesDialog(QWidget *parent, Project *project) :
    QDialog(parent),
    ui(new Ui::ProjectPropertiesDialog),
    project(project)
{
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

void ProjectPropertiesDialog::on_btnBox_clicked(QAbstractButton *button)
{
    if (ui->btnBox->standardButton(button) == QDialogButtonBox::Cancel) {
        this->close();
        return;
    }

    if (isFormValid()) {
        QString name = ui->edtName->text();
        QString description = ui->txtDescription->toPlainText();
        bool hydrodynamic = ui->cbxHydrodynamic->isChecked();
        bool sediment = ui->cbxSediment->isChecked();
        bool waterQuality = ui->cbxWaterQuality->isChecked();

        ProjectService projectService;
        projectService.updateProperties(name, description, hydrodynamic, sediment, waterQuality);

        this->close();
    }
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
