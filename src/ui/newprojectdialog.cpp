#include "../../include/ui/newprojectdialog.h"
#include "ui_newprojectdialog.h"

IPH::UI::NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);
}

IPH::UI::NewProjectDialog::~NewProjectDialog() {
    delete ui;
}
