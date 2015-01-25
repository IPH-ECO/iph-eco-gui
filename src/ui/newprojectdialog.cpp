#include "include/ui/newprojectdialog.h"
#include "ui_newprojectdialog.h"

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog),
    projectService(new ProjectService())
{
    ui->setupUi(this);
}

void NewProjectDialog::on_bottomButtonBox_clicked(QAbstractButton *button) {
    if (button->text() == "Save" && isFormValid()) {
        QString projectFileName = QFileDialog::getSaveFileName(this, tr("Save IPH-ECO Project"), QDir::homePath(), tr("IPH-ECHO Project file (*.iph)"));

        if (!projectFileName.isEmpty()) {
            QString projectName = ui->projectName->text();
            QString projectDescription = ui->projectDescription->toPlainText();
            QString projectAnalysis = this->formatProjectAnalysis();

            try {
                projectService->create(projectFileName, projectName, projectDescription, projectAnalysis);
                this->hide();
                this->clearForm();
            } catch (DatabaseException ex) {
                QMessageBox::critical(this, "New Project", ex.what());
            }
        }
    } else {
        if (button->text() == "Cancel") {
            this->hide();
            this->clearForm();
        }
    }
}

bool NewProjectDialog::isFormValid() {
    if (ui->projectName->text().isEmpty()) {
        QMessageBox::warning(this, tr("New Project"), tr("Project name can't be blank."));
        return false;
    }

    if (!ui->analyseHydrodynamic->isChecked() && !ui->analyseWaterQuality->isChecked() && !ui->analyseSediment->isChecked()) {
        QMessageBox::warning(this, tr("New Project"), tr("At least one analysis type must be checked"));
        return false;
    }

    return true;
}

QString NewProjectDialog::formatProjectAnalysis() {
    QStringList projectAnalysis;

    if (ui->analyseHydrodynamic->isChecked()) {
        projectAnalysis << "Hydrodynamic";
    }
    if (ui->analyseWaterQuality->isChecked()) {
        projectAnalysis << "Water Quality";
    }
    if (ui->analyseSediment->isChecked()) {
        projectAnalysis << "Sediment";
    }

    return projectAnalysis.join(",");
}

void NewProjectDialog::clearForm() {
    ui->projectName->clear();
    ui->projectDescription->clear();
    ui->analyseHydrodynamic->setChecked(false);
    ui->analyseWaterQuality->setChecked(false);
    ui->analyseWaterQuality->setChecked(false);
    ui->projectName->setFocus();
}

NewProjectDialog::~NewProjectDialog() {
    delete projectService;
    delete ui;
}
