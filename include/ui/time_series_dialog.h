#ifndef TIME_SERIES_DIALOG_H
#define TIME_SERIES_DIALOG_H

#include <domain/boundary_condition.h>
#include <domain/meteorological_parameter.h>

#include <QList>
#include <QDialog>
#include <QWidget>
#include <QSettings>
#include <QTableWidgetItem>

namespace Ui {
	class TimeSeriesDialog;
}

class TimeSeriesDialog : public QDialog {
    Q_OBJECT
private:
    const QString HYDRODYNAMIC_DEFAULT_DIR_KEY;
    const QString DATE_TIME_FORMAT;
    const int ITEMS_PER_PAGE;

    QSettings *appSettings;
	Ui::TimeSeriesDialog *ui;
    BoundaryCondition *currentBoundaryCondition;
    MeteorologicalParameter *currentMeteorologicalParameter;
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
    
    QList<TimeSeries*>* getTimeSeriesList() const;
    void setBoundaryCondition(BoundaryCondition *boundaryCondition);
    void setMeteorologicalParameter(MeteorologicalParameter *meteorologicalParameter);
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