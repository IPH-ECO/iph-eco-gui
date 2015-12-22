#include <ui/time_series_chart_dialog.h>
#include "ui_time_series_chart_dialog.h"

#include <QToolButton>

TimeSeriesChartDialog::TimeSeriesChartDialog(QWidget *parent, Simulation *simulation, const int &numberOfMapLayers) :
	QDialog(parent),
	ui(new Ui::TimeSeriesChartDialog),
    simulation(simulation)
{
    ui->setupUi(this);
    ui->edtInitialFrame->setText("1");
    ui->edtFrameStep->setText("1");
    ui->edtFinalFrame->setText(QString::number(simulation->getOutputFiles().size()));
    
    for (int i = 1; i <= numberOfMapLayers; i++) {
        ui->cbxLayer->addItem(QString::number(i));
    }
    
    QToolButton *btnExportToCSV = new QToolButton(this);
    btnExportToCSV->setIcon(QIcon(":/icons/text-csv.png"));
    btnExportToCSV->setToolTip("Export chart data to CSV");
    ui->buttonBox->addButton(btnExportToCSV, QDialogButtonBox::ActionRole);
    connect(btnExportToCSV, SIGNAL(clicked()), this, SLOT(exportChartToCSV()));
}

TimeSeriesChartDialog::~TimeSeriesChartDialog() {
    delete ui;
}

void TimeSeriesChartDialog::on_btnBrowseShapefile_clicked() {
    
}

void TimeSeriesChartDialog::on_btnPlot_clicked() {
    
}

void TimeSeriesChartDialog::on_buttonBox_clicked(QAbstractButton *button) {
    QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);

    if (standardButton == QDialogButtonBox::Cancel) {
        QDialog::reject();
    }
}