#include "include/ui/save_on_close_dialog.h"
#include "ui_save_on_close_dialog.h"

SaveOnCloseDialog::SaveOnCloseDialog(QWidget *parent, Project *project) :
    QDialog(parent),
    ui(new Ui::SaveOnCloseDialog),
    project(project)
{
    ui->setupUi(this);
}

SaveOnCloseDialog::~SaveOnCloseDialog()
{
    delete ui;
}

void SaveOnCloseDialog::on_buttonBox_accepted()
{
     ProjectService projectService;
     projectService.save(this->project);
}
