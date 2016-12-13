#ifndef TIME_SERIES_DIALOG_H
#define TIME_SERIES_DIALOG_H

#include <domain/time_series.h>

#include <QList>
#include <QDialog>
#include <QWidget>
#include <QSettings>
#include <QTableWidgetItem>

namespace Ui {
	class TimeSeriesDialog;
}

enum class TimeSeriesObjectType { BOUNDARY_CONDITION = 1, METEOROLOGICAL_PARAMETER, VERTICAL_INTEGRATED_RANGE };

class TimeSeriesDialog : public QDialog {
    Q_OBJECT
private:
    const QString TIME_SERIES_DEFAULT_DIR_KEY;
    const QString DATE_TIME_FORMAT;
    const int ITEMS_PER_PAGE;

    TimeSeriesObjectType objectType;
    QSettings *appSettings;
	Ui::TimeSeriesDialog *ui;
    QList<TimeSeries> copyTimeSeriesList;
    QList<TimeSeries*>* originalTimeSeriesList;
    TimeSeriesType timeSeriesType;
    int currentPage;
    int pagesTotal;
    bool changed;
    
    QString getDefaultDirectory();
public:
	explicit TimeSeriesDialog(QWidget *parent, const TimeSeriesType &timeSeriesType);
    ~TimeSeriesDialog();
    
    void setObjectType(const TimeSeriesObjectType &objectType);
    QList<TimeSeries*>* getTimeSeriesList() const;
    void loadTimeSeriesList(QList<TimeSeries*> *timeSeriesList);
    virtual void accept();
    bool hasChanges() const;
private slots:
    void on_btnImportCSV_clicked();
    void on_btnClear_clicked();
    void on_btnFirstPage_clicked();
    void on_btnPreviousPage_clicked();
    void on_btnNextPage_clicked();
    void on_btnLastPage_clicked();
    void on_spxCurrentPage_valueChanged(int page);
    void on_tblTimeSeries_itemChanged(QTableWidgetItem *item);
};

#endif // TIME_SERIES_DIALOG_H
