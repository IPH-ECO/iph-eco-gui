#include "../../include/ui/projectpropertiesdialog.h"
#include "ui_projectpropertiesdialog.h"

IPH::UI::ProjectPropertiesDialog::ProjectPropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProjectPropertiesDialog)
{
    ui->setupUi(this);
}

IPH::UI::ProjectPropertiesDialog::~ProjectPropertiesDialog() {
    delete ui;
}
