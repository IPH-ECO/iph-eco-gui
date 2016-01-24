#include <ui/about_dialog.h>
#include "ui_about_dialog.h"
#include <application/version.h>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
	ui->lblVersion->setText(QString("Version: %1").arg(VER_PRODUCTVERSION_STR));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
