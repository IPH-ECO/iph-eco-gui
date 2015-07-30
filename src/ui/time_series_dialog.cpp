#include "include/ui/time_series_dialog.h"
#include "ui_time_series_dialog.h"

#include <QTableWidgetItem>
#include <QFileDialog>
#include <QStringList>
#include <QMessageBox>
#include <QIODevice>
#include <QFile>

enum CustomDialogButtons { IMPORT_CSV = 1 };

TimeSeriesDialog::TimeSeriesDialog(QWidget *parent) : QDialog(parent), HYDRODYNAMIC_DEFAULT_DIR_KEY("default_hydrodynamic_dir"), ui(new Ui::TimeSeriesDialog) {
    ui->setupUi(this);
    ui->tblTimeSeries->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);
}

TimeSeriesDialog::~TimeSeriesDialog() {
    delete appSettings;
    delete ui;
}

QString TimeSeriesDialog::getDefaultDirectory() {
    return appSettings->value(HYDRODYNAMIC_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(HYDRODYNAMIC_DEFAULT_DIR_KEY).toString();
}

QMap<QString, double> TimeSeriesDialog::getTimeSeries() const {
    return timeSeries;
}

void TimeSeriesDialog::on_btnAddEntry_clicked() {
    ui->tblTimeSeries->insertRow(ui->tblTimeSeries->rowCount());
}

void TimeSeriesDialog::on_btnImportCSV_clicked() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Select a CSV file"), getDefaultDirectory(), tr("Comma Separated Values file (*.csv)"));
    
    if (!filename.isEmpty()) {
        QFile csvFile(filename);
        QStringList tokens;
        
        if (csvFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMap<QString, double> tempTimeSeries;
            int i = 1;
            
            while (!csvFile.atEnd()) {
                QString line = csvFile.readLine();
                QStringList tokens = line.split(";");
                
                if (tokens.size() < 2) {
                    QMessageBox::critical(this, tr("Time Series"), QString("Parsing error at line %1.").arg(i));
                    csvFile.close();
                    return;
                } else {
                    tempTimeSeries.insert(tokens[0], tokens[1].toDouble());
                }
                i++;
            }
            
            if (tempTimeSeries.isEmpty()) {
                QMessageBox::critical(this, tr("Time Series"), tr("Unable to parse empty file."));
            } else {
                timeSeries = tempTimeSeries;
                i = 0;
                
                ui->tblTimeSeries->setRowCount(0);
                
                for (QMap<QString, double>::const_iterator it = timeSeries.begin(); it != timeSeries.end(); it++) {
                    ui->tblTimeSeries->insertRow(i);
                    ui->tblTimeSeries->setItem(i, 0, new QTableWidgetItem(it.key()));
                    ui->tblTimeSeries->setItem(i, 1, new QTableWidgetItem(QString::number(it.value())));
                    i++;
                }
            }
            
            csvFile.close();
        } else {
            QMessageBox::critical(this, tr("Time Series"), csvFile.errorString());
        }
    }
}

void TimeSeriesDialog::on_btnRemoveSelected_clicked() {
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("Time Series"), tr("Are you sure?"));
    
    if (button == QMessageBox::Yes) {
        QModelIndexList indexes = ui->tblTimeSeries->selectionModel()->selectedIndexes();

        while (indexes.size()) {
            ui->tblTimeSeries->removeRow(indexes.first().row());
            indexes = ui->tblTimeSeries->selectionModel()->selectedIndexes();
        }
    }
}

void TimeSeriesDialog::on_btnClear_clicked() {
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("Time Series"), tr("Are you sure?"));

    if (button == QMessageBox::Yes) {
        ui->tblTimeSeries->setRowCount(0);
    }
}