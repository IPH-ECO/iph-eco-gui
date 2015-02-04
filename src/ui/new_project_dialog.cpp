#include "include/ui/new_project_dialog.h"
#include "ui_new_project_dialog.h"

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);

    connect(this, SIGNAL(toggleParentMenu(bool)), parentWidget(), SLOT(toggleMenu(bool)));
}

void NewProjectDialog::on_btnBox_clicked(QAbstractButton *button) {
    if (ui->btnBox->standardButton(button) == QDialogButtonBox::Cancel) {
        this->close();
        return;
    }

    if (isFormValid()) {
        QString name = ui->edtName->text();
        QString description = ui->edtDescription->toPlainText();
        bool hydrodynamic = ui->cbxHydrodynamic->isChecked();
        bool sediment = ui->cbxSediment->isChecked();
        bool waterQuality = ui->cbxWaterQuality->isChecked();

        ProjectService projectService;
        projectService.setApplicationProject(name, description, hydrodynamic, sediment, waterQuality);

        emit toggleParentMenu(true);

        this->close();
    }
}

bool NewProjectDialog::isFormValid() {
    if (ui->edtName->text().isEmpty()) {
        QMessageBox::warning(this, tr("New Project"), tr("Project name can't be blank."));
        return false;
    }

    if (!ui->cbxHydrodynamic->isChecked() && !ui->cbxWaterQuality->isChecked() && !ui->cbxSediment->isChecked()) {
        QMessageBox::warning(this, tr("New Project"), tr("At least one analysis type must be checked."));
        return false;
    }

    return true;
}

NewProjectDialog::~NewProjectDialog() {
    delete ui;
}
