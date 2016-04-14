#include <ui/new_project_dialog.h>
#include "ui_new_project_dialog.h"

#include <application/iph_application.h>
#include <QMessageBox>

NewProjectDialog::NewProjectDialog(QWidget *parent) : QDialog(parent), ui(new Ui::NewProjectDialog) {
    ui->setupUi(this);
    connect(this, SIGNAL(enableParentMenu(bool)), parent, SLOT(enableMenus(bool)));
}

void NewProjectDialog::accept() {
    if (!isFormValid()) {
        return;
    }
    
    QString name = ui->edtName->text();
    QString description = ui->edtDescription->toPlainText();
    bool hydrodynamic = ui->cbxHydrodynamic->isChecked();
    bool waterQuality = ui->cbxWaterQuality->isChecked();
    
    Project *project = new Project(name, description, hydrodynamic, waterQuality);
    IPHApplication::setCurrentProject(project);
    
    static_cast<QDialog*>(this->parent())->setWindowTitle("IPH-ECO - " + name);
    emit enableParentMenu(true);
    QDialog::accept();
}

bool NewProjectDialog::isFormValid() {
    if (ui->edtName->text().isEmpty()) {
        QMessageBox::warning(this, tr("New Project"), tr("Project name can't be blank."));
        return false;
    }

    if (!ui->cbxHydrodynamic->isChecked() && !ui->cbxWaterQuality->isChecked()) {
        QMessageBox::warning(this, tr("New Project"), tr("At least one analysis type must be checked."));
        return false;
    }

    return true;
}

NewProjectDialog::~NewProjectDialog() {
    delete ui;
}
