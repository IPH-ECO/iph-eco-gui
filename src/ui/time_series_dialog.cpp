#include "include/ui/time_series_dialog.h"
#include "ui_time_series_dialog.h"

#include <QTableWidgetItem>
#include <QFileDialog>
#include <QStringList>
#include <QMessageBox>
#include <QDateTime>
#include <QIODevice>
#include <QFile>

TimeSeriesDialog::TimeSeriesDialog(QWidget *parent, BoundaryCondition *boundaryCondition) :
    QDialog(parent), HYDRODYNAMIC_DEFAULT_DIR_KEY("default_hydrodynamic_dir"), ui(new Ui::TimeSeriesDialog)
{
    ui->setupUi(this);
    ui->tblTimeSeries->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);
    
    if (boundaryCondition) {
        QList<TimeSeries*> timeSeriesList = boundaryCondition->getTimeSeriesList();
        
        for (int i = 0; i < timeSeriesList.size(); i++) {
            ui->tblTimeSeries->insertRow(i);
            ui->tblTimeSeries->setItem(i, 0, new QTableWidgetItem(timeSeriesList[i]->getTimestamp()));
            ui->tblTimeSeries->setItem(i, 1, new QTableWidgetItem(QString::number(timeSeriesList[i]->getValue())));
            i++;
        }
    }
}

TimeSeriesDialog::~TimeSeriesDialog() {
    delete appSettings;
    delete ui;
}

QString TimeSeriesDialog::getDefaultDirectory() {
    return appSettings->value(HYDRODYNAMIC_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(HYDRODYNAMIC_DEFAULT_DIR_KEY).toString();
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
            QList<TimeSeries> tempTimeSeriesList;
            int i = 1;
            
            while (!csvFile.atEnd()) {
                QString line = csvFile.readLine();
                QStringList tokens = line.split(";");
                
                if (tokens.size() < 2) {
                    QMessageBox::critical(this, tr("Time Series"), QString("Parsing error at line %1.").arg(i));
                    csvFile.close();
                    return;
                } else {
                    TimeSeries tempTimeSeries;
                    tempTimeSeries.setTimestamp(tokens[0]);
                    tempTimeSeries.setValue(tokens[1].toDouble());
                    
                    tempTimeSeriesList.append(tempTimeSeries);
                }
                i++;
            }
            
            if (tempTimeSeriesList.isEmpty()) {
                QMessageBox::critical(this, tr("Time Series"), tr("Unable to parse empty file."));
            } else {
                ui->tblTimeSeries->setRowCount(0);
                
                for (int i = 0; i < tempTimeSeriesList.size(); i++) {
                    ui->tblTimeSeries->insertRow(i);
                    ui->tblTimeSeries->setItem(i, 0, new QTableWidgetItem(tempTimeSeriesList[i].getTimestamp()));
                    ui->tblTimeSeries->setItem(i, 1, new QTableWidgetItem(QString::number(tempTimeSeriesList[i].getValue())));
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

bool TimeSeriesDialog::isValid() {
    for (int i = 0; i < ui->tblTimeSeries->rowCount(); i++) {
        QString timestamp = ui->tblTimeSeries->item(i, 0)->text();
        QDateTime dateTime = QDateTime::fromString(timestamp, Qt::ISODate);
        
        if (!dateTime.isValid()) {
            QMessageBox::warning(this, tr("Time Series"), tr("Invalid timestamp format at line ") + QString::number(i + 1));
            return false;
        }
    }
    
    return true;
}

void TimeSeriesDialog::accept() {
    if (!isValid()) {
        return;
    }
    
    QList<TimeSeries*> timeSeriesList;
    
    for (int i = 0; i < ui->tblTimeSeries->rowCount(); i++) {
        TimeSeries *timeSeries = new TimeSeries();
        timeSeries->setTimestamp(ui->tblTimeSeries->item(i, 0)->text());
        timeSeries->setValue(ui->tblTimeSeries->item(i, 1)->text().toDouble());
        
        timeSeriesList.append(timeSeries);
    }
    
    QDialog::accept();
}

QList<TimeSeries*> TimeSeriesDialog::getTimeSeriesList() const {
    return timeSeriesList;
}