#include "include/ui/time_series_dialog.h"
#include "ui_time_series_dialog.h"

#include <QTableWidgetItem>
#include <QProgressDialog>
#include <QFileDialog>
#include <QStringList>
#include <QMessageBox>
#include <QDateTime>
#include <QIODevice>
#include <QFile>

TimeSeriesDialog::TimeSeriesDialog(QWidget *parent, const TimeSeriesType &timeSeriesType) :
    QDialog(parent), HYDRODYNAMIC_DEFAULT_DIR_KEY("default_hydrodynamic_dir"), dateTimeFormat("yyyy-MM-dd HH:mm:ss"), ui(new Ui::TimeSeriesDialog),
    currentBoundaryCondition(nullptr), currentMeteorologicalParameter(nullptr), timeSeriesList(nullptr), timeSeriesType(timeSeriesType)
{
    ui->setupUi(this);
    ui->lblLoading->hide();
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
    QProgressDialog *progressDialog = new QProgressDialog(tr("Loading time series..."), tr("Cancel"), 0, timeSeriesList.size());
    int i = 0;
    
    for (TimeSeries *timeSeries : timeSeriesList) {
        QDateTime time = QDateTime::fromTime_t(timeSeries->getTimeStamp(), Qt::UTC);
        
        if (progressDialog->wasCanceled()) {
            ui->tblTimeSeries->setRowCount(0);
            delete progressDialog;
            return;
        }
        
        progressDialog->setValue(i);
		QApplication::processEvents();
        
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
    
    delete progressDialog;
}

TimeSeriesDialog::~TimeSeriesDialog() {
    delete timeSeriesList;
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
        bool hasError = false;
        
        if (csvFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            int columnCount = timeSeriesType == TimeSeriesType::DEFAULT ? 2 : 3;
            int i = 0;
            
            ui->tblTimeSeries->setRowCount(0);
            ui->lblLoading->show();
            
            while (!csvFile.atEnd()) {
                QString line = csvFile.readLine();
                QStringList tokens = line.split(",");
                
                if (tokens.size() < columnCount) {
                    QMessageBox::critical(this, tr("Time Series"), QString("Invalid column count at line %1.").arg(i + 1));
                    hasError = true;
                    break;
                } else {
                    ui->tblTimeSeries->insertRow(i);
                    ui->tblTimeSeries->setItem(i, 0, new QTableWidgetItem(tokens[0]));
                    ui->tblTimeSeries->setItem(i, 1, new QTableWidgetItem(tokens[1]));
                    if (timeSeriesType != TimeSeriesType::DEFAULT) {
                        ui->tblTimeSeries->setItem(i, 2, new QTableWidgetItem(tokens[2]));
                    }
                }
                
                i++;
            }
            
            if (ui->tblTimeSeries->rowCount() == 0 && !hasError) {
                QMessageBox::critical(this, tr("Time Series"), tr("Unable to parse empty file."));
            }
        } else {
            QMessageBox::critical(this, tr("Time Series"), csvFile.errorString());
        }
        
        csvFile.close();
        ui->lblLoading->hide();
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

void TimeSeriesDialog::accept() {
    QDateTime lastTime;
    
    timeSeriesList = new QList<TimeSeries*>();
    
    for (int i = 0; i < ui->tblTimeSeries->rowCount(); i++) {
        QString timestamp = ui->tblTimeSeries->item(i, 0)->text();
        QDateTime dateTime = QDateTime::fromString(timestamp, Qt::ISODate);
        
        dateTime.setTimeSpec(Qt::UTC);
        
        if (!dateTime.isValid()) {
            QMessageBox::warning(this, tr("Time Series"), QString("Invalid timestamp format at row %1.").arg(i + 1));
            return;
        }
        
        if (lastTime.isValid() && dateTime < lastTime) {
            QMessageBox::warning(this, tr("Time Series"), QString("The time stamps must be on ascendant order. See row %1.").arg(i + 1));
            return;
        }
        
        lastTime = dateTime;
        
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
        
        timeSeries->setTimeStamp(dateTime.toTime_t());
        timeSeries->setValue1(ui->tblTimeSeries->item(i, 1)->text().toDouble());
        if (timeSeriesType != TimeSeriesType::DEFAULT) {
            timeSeries->setValue2(ui->tblTimeSeries->item(i, 2)->text().toDouble());
        }
        timeSeries->setObjectType(currentBoundaryCondition ? "BoundaryCondition" : "MeteorologicalParameter");
        
        timeSeriesList->append(timeSeries);
    }
    
    QDialog::accept();
}

QList<TimeSeries*>* TimeSeriesDialog::getTimeSeriesList() const {
    return timeSeriesList;
}

void TimeSeriesDialog::setBoundaryCondition(BoundaryCondition *boundaryCondition) {
    currentBoundaryCondition = boundaryCondition;
}

void TimeSeriesDialog::setMeteorologicalParameter(MeteorologicalParameter *meteorologicalParameter) {
    currentMeteorologicalParameter = meteorologicalParameter;
}