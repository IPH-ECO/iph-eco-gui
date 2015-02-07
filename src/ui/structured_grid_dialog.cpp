#include "include/ui/structured_grid_dialog.h"
#include "ui_structured_grid_dialog.h"

#include <QDebug>

StructuredGridDialog::StructuredGridDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StructuredGridDialog)
{
    ui->setupUi(this);
}

void StructuredGridDialog::on_boundaryFileButton_clicked() {
    QString filepath = QFileDialog::getOpenFileName(this, tr("Select a boundary file"), ".", "Keyhole Markup Language files (*.kml)");

    if (!filepath.isEmpty()) {
        ui->boundaryFileTextEdit->setText(filepath);

//        try {
//            MeshService gridService;
//            QString boundaryJson = gridService.getBoundaryJson();

//            qDebug() << boundaryJson;
//            Project *project = IPHApplication::getCurrentProject();
//            project->setBoundaryData(boundaryJson);
//        } catch (MeshException &ex) {
//            QMessageBox::critical(this, "Structured Grid Generation", ex.what());
//        }
    }
}

void StructuredGridDialog::on_bottomButtonBox_clicked(QAbstractButton *button) {
    if (button->text() == "OK" && isValidForm()) {
        // TODO:
        clearForm();
    } else {
        if (button->text() == "Cancel") {
            clearForm();
        }
    }
}

bool StructuredGridDialog::isValidForm() {
    if (ui->boundaryFileTextEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("Structured Grid Generation"), tr("Select a boundary file."));
        return false;
    }

    return true;
}

void StructuredGridDialog::clearForm() {
    this->hide();
    ui->boundaryFileTextEdit->clear();
}

StructuredGridDialog::~StructuredGridDialog() {
    delete ui;
}
