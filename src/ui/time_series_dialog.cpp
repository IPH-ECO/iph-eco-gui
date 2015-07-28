#include "include/ui/time_series_dialog.h"
#include "ui_time_series_dialog.h"

#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

enum CustomDialogButtons { IMPORT_CSV = 1 };

TimeSeriesDialog::TimeSeriesDialog(QWidget *parent) : QDialog(parent), HYDRODYNAMIC_DEFAULT_DIR_KEY("default_hydrodynamic_dir"), ui(new Ui::TimeSeriesDialog) {
    ui->setupUi(this);
    ui->tblTimeSeries->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QPushButton *btnClear = new QPushButton(ui->buttonBox);
    btnClear->setText("Clear");
    ui->buttonBox->addButton(btnClear, QDialogButtonBox::ActionRole);
    connect(btnClear, SIGNAL(clicked()), this, SLOT(on_btnClear_clicked()));
    
    QPushButton *btnImportCSV = new QPushButton(ui->buttonBox);
    btnImportCSV->setText("Import CSV...");
    ui->buttonBox->addButton(btnImportCSV, QDialogButtonBox::ActionRole);
    connect(btnImportCSV, SIGNAL(clicked()), this, SLOT(on_btnImportCSV_clicked()));
    
    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);
}

TimeSeriesDialog::~TimeSeriesDialog() {
    delete ui;
}

QString TimeSeriesDialog::getDefaultDirectory() {
    return appSettings->value(HYDRODYNAMIC_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(HYDRODYNAMIC_DEFAULT_DIR_KEY).toString();
}

void TimeSeriesDialog::on_btnImportCSV_clicked() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Select a CSV file"), getDefaultDirectory(), tr("Comma Separated Values file (*.csv)"));
}

void TimeSeriesDialog::on_btnClear_clicked() {
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("Time Series"), tr("Are you sure?"));

    if (button == QMessageBox::Yes) {
        ui->tblTimeSeries->clearContents();
    }
}
