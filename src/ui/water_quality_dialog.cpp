#include <ui/water_quality_dialog.h>
#include "ui_water_quality_dialog.h"

#include <QUrl>
#include <QWebFrame>

WaterQualityDialog::WaterQualityDialog(QWidget *parent) :
	QDialog(parent),
    ui(new Ui::WaterQualityDialog)
{
	ui->setupUi(this);
    ui->webView->setUrl(QUrl("file:///Users/joaoroberto/Downloads/inputDiagram.html"));
    
    QObject::connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(toggleElements(bool)));
}

void WaterQualityDialog::on_btnHide_clicked() {
    ui->webView->page()->mainFrame()->evaluateJavaScript("toggleElements('reaeration')");
}