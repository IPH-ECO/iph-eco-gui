#include <ui/time_series_dialog.h>
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
    QDialog(parent),
    HYDRODYNAMIC_DEFAULT_DIR_KEY("default_hydrodynamic_dir"),
    DATE_TIME_FORMAT("yyyy-MM-dd HH:mm:ss"),
    ITEMS_PER_PAGE(500),
    ui(new Ui::TimeSeriesDialog),
    currentBoundaryCondition(nullptr),
    currentMeteorologicalParameter(nullptr),
    timeSeriesType(timeSeriesType),
    changed(false)
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

TimeSeriesDialog::~TimeSeriesDialog() {
    delete appSettings;
    delete ui;
}

void TimeSeriesDialog::loadTimeSeriesList(QList<TimeSeries*> *timeSeriesList) {
    int timeSeriesListSize = timeSeriesList->size();
    
    originalTimeSeriesList = timeSeriesList;

    if (timeSeriesListSize > 0) {
        for (TimeSeries *timeSeries : *timeSeriesList) {
            this->copyTimeSeriesList.append(*timeSeries);
        }
        
        pagesTotal = timeSeriesListSize / ITEMS_PER_PAGE + (timeSeriesListSize % ITEMS_PER_PAGE > 0 ? 1 : 0);
        ui->spxCurrentPage->setMaximum(pagesTotal);
        ui->lblPagesTotal->setText(QString("of %1").arg(pagesTotal));
        on_spxCurrentPage_valueChanged(1);
    }
}

void TimeSeriesDialog::accept() {
    if (changed) {
        for (TimeSeries *timeSeries : *originalTimeSeriesList) {
            delete timeSeries;
        }
        originalTimeSeriesList->clear();
        
        for (int i = 0; i < copyTimeSeriesList.size(); i++) {
            TimeSeries *newTimeSeries = new TimeSeries();
            TimeSeries *copyTimeSeries = &copyTimeSeriesList[i];
            
            newTimeSeries->setTimeStamp(copyTimeSeries->getTimeStamp());
            newTimeSeries->setValue1(copyTimeSeries->getValue1());
            newTimeSeries->setValue2(copyTimeSeries->getValue2());
            newTimeSeries->setTimeSeriesType(copyTimeSeries->getTimeSeriesType());
            
            originalTimeSeriesList->append(newTimeSeries);
        }
    }
    
    QDialog::accept();
}

QString TimeSeriesDialog::getDefaultDirectory() {
    return appSettings->value(HYDRODYNAMIC_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(HYDRODYNAMIC_DEFAULT_DIR_KEY).toString();
}

void TimeSeriesDialog::on_btnImportCSV_clicked() {
    if (ui->tblTimeSeries->rowCount() > 0) {
        QString question = tr("This action will replace the current contents of the table. Are you sure?");
        QMessageBox::StandardButton button = QMessageBox::question(this, tr("Time Series"), question);
        
        if (button != QMessageBox::Yes) {
            return;
        }
    }
    
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select a CSV file"), getDefaultDirectory(), tr("Comma Separated Values file (*.csv)"));
    
    if (!fileName.isEmpty()) {
        QFile csvFile(fileName);
        bool hasError = false;
        QStringList tokens;
        
        appSettings->setValue(HYDRODYNAMIC_DEFAULT_DIR_KEY, QFileInfo(fileName).absoluteDir().absolutePath());
        
        if (csvFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            int columnCount = timeSeriesType == TimeSeriesType::DEFAULT ? 2 : 3;
            QDateTime lastTime;
            int page = 1;
            int i = 0;
            
            while (!csvFile.atEnd()) {
                QString line = csvFile.readLine();
                QStringList tokens = line.split(",");
                
                if (tokens.size() < columnCount) {
                    QMessageBox::critical(this, tr("Time Series"), QString("Invalid column count at line %1, page %2.").arg(i).arg(page));
                    hasError = true;
                    break;
                } else {
                    QDateTime dateTime = QDateTime::fromString(tokens[0], Qt::ISODate);
                    
                    dateTime.setTimeSpec(Qt::UTC);
                    
                    if (!dateTime.isValid()) {
                        QMessageBox::warning(this, tr("Time Series"), QString("Invalid time stamp format at row %1, page %2.").arg(i).arg(page));
                        hasError = true;
                        break;
                    }
                    
                    if (lastTime.isValid() && dateTime < lastTime) {
                        QMessageBox::warning(this, tr("Time Series"), QString("The time stamps must be on ascendant order. See row %1, page %2.").arg(i).arg(page));
                        hasError = true;
                        break;
                    }
                    
                    lastTime = dateTime;
                    
                    TimeSeries timeSeries;
                    timeSeries.setTimeStamp(dateTime.toTime_t());
                    timeSeries.setValue1(tokens[1].toDouble());
                    if (timeSeriesType != TimeSeriesType::DEFAULT) {
                        timeSeries.setValue2(tokens[2].toDouble());
                    }
                    timeSeries.setObjectType(currentBoundaryCondition ? "BoundaryCondition" : "MeteorologicalParameter");
                    
                    copyTimeSeriesList.append(timeSeries);
                }
                
                i++;
                page = i / ITEMS_PER_PAGE + 1;
            }
            
            if (i > 0) {
                int itemsTotal = i + 1;
                
                changed = true;
                pagesTotal = itemsTotal / ITEMS_PER_PAGE + (itemsTotal % ITEMS_PER_PAGE > 0 ? 1 : 0);
                ui->spxCurrentPage->setMaximum(pagesTotal);
                ui->lblPagesTotal->setText(QString("of %1").arg(pagesTotal));
                on_spxCurrentPage_valueChanged(1);
                
                if (hasError) {
                    copyTimeSeriesList.removeLast();
                }
            } else if (!hasError) {
                QMessageBox::critical(this, tr("Time Series"), tr("Unable to parse empty file."));
            }
        } else {
            QMessageBox::critical(this, tr("Time Series"), csvFile.errorString());
        }
        
        csvFile.close();
    }
}

void TimeSeriesDialog::on_tblTimeSeries_itemChanged(QTableWidgetItem *item) {
    int i = (currentPage - 1) * ITEMS_PER_PAGE + item->row();
    
    if (item->column() == 0) { // Time column
        QDateTime currentDateTime = QDateTime::fromString(item->text(), Qt::ISODate);
        bool rollback = false;
        
        currentDateTime.setTimeSpec(Qt::UTC);
        
        if (!currentDateTime.isValid()) {
            QMessageBox::critical(this, tr("Time Series"), tr("Invalid time stamp format for %1").arg(currentDateTime.toString(DATE_TIME_FORMAT)));
            rollback = true;
        }
        
        QTableWidgetItem *previousItem = ui->tblTimeSeries->item(item->row() - 1, 0);
        QTableWidgetItem *nextItem = ui->tblTimeSeries->item(item->row() + 1, 0);
        QDateTime previousDateTime;
        QDateTime nextDateTime;
        
        if (previousItem) {
            previousDateTime = QDateTime::fromString(previousItem->text(), Qt::ISODate);
            previousDateTime.setTimeSpec(Qt::UTC);
            
            if (currentDateTime < previousDateTime) {
                QString message = tr("The inserted time stamp must be greater than its previous, %1.").arg(previousDateTime.toString(DATE_TIME_FORMAT));
                QMessageBox::critical(this, tr("Time Series"), message);
                rollback = true;
            }
        }
        if (nextItem) {
            nextDateTime = QDateTime::fromString(nextItem->text(), Qt::ISODate);
            nextDateTime.setTimeSpec(Qt::UTC);
            
            if (currentDateTime > nextDateTime) {
                QString message = tr("The inserted time stamp must be lesser than its next, %1.").arg(nextDateTime.toString(DATE_TIME_FORMAT));
                QMessageBox::critical(this, tr("Time Series"), message);
                rollback = true;
            }
        }
        
        TimeSeries *timeSeries = &copyTimeSeriesList[i];
        
        if (rollback) {
            currentDateTime = QDateTime::fromTime_t(timeSeries->getTimeStamp(), Qt::UTC);
            ui->tblTimeSeries->blockSignals(true);
            item->setText(currentDateTime.toString(DATE_TIME_FORMAT));
            ui->tblTimeSeries->blockSignals(false);
        } else {
            timeSeries->setTimeStamp(currentDateTime.toTime_t());
            changed = true;
        }
    } else {
        // scalar columns
        TimeSeries *timeSeries = &copyTimeSeriesList[i];
        double value = item->text().toDouble();
        
        changed = true;
        
        if (item->column() == 1) {
            timeSeries->setValue1(value);
        } else if (item->column() == 2) {
            timeSeries->setValue2(value);
        }
    }
}

void TimeSeriesDialog::on_btnClear_clicked() {
    QString question = tr("This action will remove all time series entries. Are you sure?");
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("Time Series"), question);

    if (button == QMessageBox::Yes) {
        changed = true;
        copyTimeSeriesList.clear();
        ui->tblTimeSeries->setRowCount(0);
        ui->lblPagesTotal->setText("of 1");
        ui->spxCurrentPage->setValue(1);
        ui->spxCurrentPage->setMaximum(1);
    }
}

QList<TimeSeries*>* TimeSeriesDialog::getTimeSeriesList() const {
    return originalTimeSeriesList;
}

void TimeSeriesDialog::setBoundaryCondition(BoundaryCondition *boundaryCondition) {
    currentBoundaryCondition = boundaryCondition;
}

void TimeSeriesDialog::setMeteorologicalParameter(MeteorologicalParameter *meteorologicalParameter) {
    currentMeteorologicalParameter = meteorologicalParameter;
}

void TimeSeriesDialog::on_btnFirstPage_clicked() {
    ui->spxCurrentPage->setValue(1);
}

void TimeSeriesDialog::on_btnPreviousPage_clicked() {
    ui->spxCurrentPage->setValue(currentPage == 1 ? 1 : currentPage - 1);
}

void TimeSeriesDialog::on_btnNextPage_clicked() {
    ui->spxCurrentPage->setValue(currentPage == pagesTotal ? pagesTotal : currentPage + 1);
}

void TimeSeriesDialog::on_btnLastPage_clicked() {
    ui->spxCurrentPage->setValue(pagesTotal);
}

void TimeSeriesDialog::on_spxCurrentPage_valueChanged(int page) {
    int startIndex = (page - 1) * ITEMS_PER_PAGE;
    int endIndex = startIndex + ITEMS_PER_PAGE;
    int row = 0;
    
    if (endIndex > copyTimeSeriesList.size()) {
        endIndex = startIndex + copyTimeSeriesList.size() % ITEMS_PER_PAGE;
    }

    currentPage = page;
    ui->tblTimeSeries->clearContents();
    ui->tblTimeSeries->setRowCount(0);
    
    ui->tblTimeSeries->blockSignals(true);
    
    for (int i = startIndex; i < endIndex; i++, row++) {
        TimeSeries *timeSeries = &copyTimeSeriesList[i];
        QDateTime dateTime = QDateTime::fromTime_t(timeSeries->getTimeStamp(), Qt::UTC);
        
        ui->tblTimeSeries->insertRow(row);
        ui->tblTimeSeries->setItem(row, 0, new QTableWidgetItem(dateTime.toString(DATE_TIME_FORMAT)));
        ui->tblTimeSeries->setItem(row, 1, new QTableWidgetItem(QString::number(timeSeries->getValue1())));
        if (timeSeriesType != TimeSeriesType::DEFAULT) {
            ui->tblTimeSeries->setItem(row, 2, new QTableWidgetItem(QString::number(timeSeries->getValue2())));
        }
        ui->tblTimeSeries->setVerticalHeaderItem(row, new QTableWidgetItem(QString::number(i + 1)));
    }
    ui->tblTimeSeries->blockSignals(false);
}

bool TimeSeriesDialog::hasChanges() const {
    return changed;
}