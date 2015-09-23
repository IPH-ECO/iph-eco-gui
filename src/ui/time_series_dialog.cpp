#include "include/ui/time_series_dialog.h"
#include "ui_time_series_dialog.h"

#include <QTableWidgetItem>
#include <QFileDialog>
#include <QStringList>
#include <QMessageBox>
#include <QDateTime>
#include <QIODevice>
#include <QFile>

TimeSeriesDialog::TimeSeriesDialog(QWidget *parent, const TimeSeriesType &timeSeriesType) :
    QDialog(parent), HYDRODYNAMIC_DEFAULT_DIR_KEY("default_hydrodynamic_dir"), dateTimeFormat("yyyy-MM-dd HH:mm:ss"), ui(new Ui::TimeSeriesDialog),
    currentBoundaryCondition(nullptr), currentMeteorologicalParameter(nullptr), timeSeriesType(timeSeriesType)
{
    ui->setupUi(this);
    ui->tblTimeSeries->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);
    
    if (timeSeriesType == TimeSeriesType::XY_COMPONENTS) {
        ui->tblTimeSeries->insertColumn(2);
        ui->tblTimeSeries->setHorizontalHeaderItem(1, new QTableWidgetItem("X Component"));
        ui->tblTimeSeries->setHorizontalHeaderItem(2, new QTableWidgetItem("Y Component"));
    } else if (timeSeriesType == TimeSeriesType::INTENSITY_DIRECTION) {
        ui->tblTimeSeries->insertColumn(2);
        ui->tblTimeSeries->setHorizontalHeaderItem(1, new QTableWidgetItem("Intensity"));
        ui->tblTimeSeries->setHorizontalHeaderItem(2, new QTableWidgetItem("Direction"));
    }
}

void TimeSeriesDialog::loadTimeSeriesList(const QList<TimeSeries*> &timeSeriesList) {
    this->timeSeriesList = timeSeriesList;
    int i = 0;
    
    for (TimeSeries *timeSeries : timeSeriesList) {
        QDateTime time = QDateTime::fromTime_t(timeSeries->getTimeStamp(), Qt::UTC);
        
        ui->tblTimeSeries->insertRow(i);
        ui->tblTimeSeries->setItem(i, 0, new QTableWidgetItem(time.toString(dateTimeFormat)));
        ui->tblTimeSeries->setItem(i, 1, new QTableWidgetItem(QString::number(timeSeries->getValue1())));
        if (timeSeriesType != TimeSeriesType::DEFAULT) {
            ui->tblTimeSeries->setItem(i, 2, new QTableWidgetItem(QString::number(timeSeries->getValue2())));
        }
        
        if (timeSeries->isPersisted()) {
            ui->tblTimeSeries->item(i, 0)->setData(Qt::UserRole, QVariant((uint) timeSeries->getId()));
        }
        
        i++;
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
        appSettings->setValue(HYDRODYNAMIC_DEFAULT_DIR_KEY, QFileInfo(filename).absoluteDir().absolutePath());
        
        QFile csvFile(filename);
        QStringList tokens;
        
        if (csvFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            int columnCount = timeSeriesType == TimeSeriesType::DEFAULT ? 2 : 3;
            QList<TimeSeries> tempTimeSeriesList;
            int i = 1;
            
            while (!csvFile.atEnd()) {
                QString line = csvFile.readLine();
                QStringList tokens = line.split(",");
                
                if (tokens.size() < columnCount) {
                    QMessageBox::critical(this, tr("Time Series"), QString("Invalid column count at line %1.").arg(i));
                    csvFile.close();
                    return;
                } else {
                    TimeSeries tempTimeSeries;
                    QDateTime timeStamp = QDateTime::fromString(tokens[0], dateTimeFormat);
                    
                    timeStamp.setTimeSpec(Qt::UTC);
                    tempTimeSeries.setTimeStamp(timeStamp.toTime_t());
                    tempTimeSeries.setValue1(tokens[1].toDouble());
                    if (timeSeriesType != TimeSeriesType::DEFAULT) {
                        tempTimeSeries.setValue2(tokens[2].toDouble());
                    }
                    
                    tempTimeSeriesList.append(tempTimeSeries);
                }
                i++;
            }
            
            if (tempTimeSeriesList.isEmpty()) {
                QMessageBox::critical(this, tr("Time Series"), tr("Unable to parse empty file."));
            } else {
                ui->tblTimeSeries->setRowCount(0);
                
                for (int i = 0; i < tempTimeSeriesList.size(); i++) {
                    QString timeStamp = QDateTime::fromTime_t(tempTimeSeriesList[i].getTimeStamp(), Qt::UTC).toString(dateTimeFormat);
                    ui->tblTimeSeries->insertRow(i);
                    ui->tblTimeSeries->setItem(i, 0, new QTableWidgetItem(timeStamp));
                    ui->tblTimeSeries->setItem(i, 1, new QTableWidgetItem(QString::number(tempTimeSeriesList[i].getValue1())));
                    if (timeSeriesType != TimeSeriesType::DEFAULT) {
                        ui->tblTimeSeries->setItem(i, 2, new QTableWidgetItem(QString::number(tempTimeSeriesList[i].getValue2())));
                    }
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
    QDateTime lastTime;
    
    for (int i = 0; i < ui->tblTimeSeries->rowCount(); i++) {
        QString timestamp = ui->tblTimeSeries->item(i, 0)->text();
        QDateTime dateTime = QDateTime::fromString(timestamp, Qt::ISODate);
        
        if (!dateTime.isValid()) {
            QMessageBox::warning(this, tr("Time Series"), QString("Invalid timestamp format at row %1.").arg(i + 1));
            return false;
        }
        
        if (lastTime.isValid() && dateTime < lastTime) {
            QMessageBox::warning(this, tr("Time Series"), QString("The time stamps must be on ascendant order. See row %1.").arg(i + 1));
            return false;
        }
        
        lastTime = dateTime;
    }
    
    return true;
}

void TimeSeriesDialog::accept() {
    if (!isValid()) {
        return;
    }
    
    timeSeriesList.clear();
    
    for (int i = 0; i < ui->tblTimeSeries->rowCount(); i++) {
        QVariant data = ui->tblTimeSeries->item(i, 0)->data(Qt::UserRole);
        TimeSeries *timeSeries = nullptr;
        
        if (data.isValid()) {
            if (currentBoundaryCondition) {
                timeSeries = currentBoundaryCondition->getTimeSeries(data.toUInt());
            } else {
                timeSeries = currentMeteorologicalParameter->getTimeSeries(data.toUInt());
            }
        } else {
            timeSeries = new TimeSeries();
        }
        
        QDateTime time = QDateTime::fromString(ui->tblTimeSeries->item(i, 0)->text(), dateTimeFormat);
        time.setTimeSpec(Qt::UTC);
        
        timeSeries->setTimeStamp(time.toTime_t());
        timeSeries->setValue1(ui->tblTimeSeries->item(i, 1)->text().toDouble());
        if (timeSeriesType != TimeSeriesType::DEFAULT) {
            timeSeries->setValue2(ui->tblTimeSeries->item(i, 2)->text().toDouble());
        }
        timeSeries->setObjectType(currentBoundaryCondition ? "BoundaryCondition" : "MeteorologicalParameter");
        
        timeSeriesList.append(timeSeries);
    }
    
    QDialog::accept();
}

QList<TimeSeries*> TimeSeriesDialog::getTimeSeriesList() const {
    return timeSeriesList;
}

void TimeSeriesDialog::setBoundaryCondition(BoundaryCondition *boundaryCondition) {
    currentBoundaryCondition = boundaryCondition;
}

void TimeSeriesDialog::setMeteorologicalParameter(MeteorologicalParameter *meteorologicalParameter) {
    currentMeteorologicalParameter = meteorologicalParameter;
}