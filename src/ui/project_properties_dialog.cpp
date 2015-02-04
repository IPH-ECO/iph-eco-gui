#include "include/ui/project_properties_dialog.h"
#include "ui_project_properties_dialog.h"

ProjectPropertiesDialog::ProjectPropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProjectPropertiesDialog)
{
    ui->setupUi(this);

    ui->edtName->setText(IPHApplication::getCurrentProject()->getName());
    ui->txtDescription->setPlainText(IPHApplication::getCurrentProject()->getDescription());
    ui->cbxHydrodynamic->setChecked(IPHApplication::getCurrentProject()->getHydrodynamic());
    ui->cbxSediment->setChecked(IPHApplication::getCurrentProject()->getSediment());
    ui->cbxWaterQuality->setChecked(IPHApplication::getCurrentProject()->getWaterQuality());
}

ProjectPropertiesDialog::~ProjectPropertiesDialog() {
    delete ui;
}

void ProjectPropertiesDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if (button->text() == "OK") {
        QString name = ui->edtName->text();
        QString description = ui->txtDescription->toPlainText();
        bool hydrodynamic = ui->cbxHydrodynamic->isChecked();
        bool sediment = ui->cbxSediment->isChecked();
        bool waterQuality = ui->cbxWaterQuality->isChecked();

        ProjectService projectService;
        projectService.updateProperties(name, description, hydrodynamic, sediment, waterQuality);
    }

    this->close();
}
