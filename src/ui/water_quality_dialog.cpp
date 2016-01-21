#include <ui/water_quality_dialog.h>
#include "ui_water_quality_dialog.h"

#include <QUrl>

WaterQualityDialog::WaterQualityDialog(QWidget *parent) :
	QDialog(parent),
    ui(new Ui::WaterQualityDialog)
{
	ui->setupUi(this);
    ui->webView->setUrl(QUrl("qrc:/data/water_quality_diagram.html"));
}