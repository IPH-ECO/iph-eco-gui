#include <ui/time_series_chart_dialog.h>
#include "ui_time_series_chart_dialog.h"

#include <QToolButton>

TimeSeriesChartDialog::TimeSeriesChartDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::TimeSeriesChartDialog)
{
    ui->setupUi(this);
    
    QToolButton *btnExportToCSV = new QToolButton(this);
    btnExportToCSV->setIcon(QIcon(":/icons/text-csv.png"));
    btnExportToCSV->setToolTip("Export chart data to CSV");
    ui->buttonBox->addButton(btnExportToCSV, QDialogButtonBox::ActionRole);
    connect(btnExportToCSV, SIGNAL(clicked()), this, SLOT(exportChartToCSV()));
}

TimeSeriesChartDialog::~TimeSeriesChartDialog() {
	delete ui;
}