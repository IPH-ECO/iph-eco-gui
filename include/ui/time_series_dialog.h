#ifndef TIME_SERIES_DIALOG_H
#define TIME_SERIES_DIALOG_H

#include "include/domain/boundary_condition.h"

#include <QSettings>
#include <QDialog>
#include <QWidget>
#include <QList>

namespace Ui {
	class TimeSeriesDialog;
}

class TimeSeriesDialog : public QDialog {
    Q_OBJECT
private:
    const QString HYDRODYNAMIC_DEFAULT_DIR_KEY;
    const QString dateTimeFormat;

    QSettings *appSettings;
	Ui::TimeSeriesDialog *ui;
    BoundaryCondition *currentBoundaryCondition;
    QList<TimeSeries*> timeSeriesList;
    TimeSeriesType timeSeriesType;
    
    QString getDefaultDirectory();
    bool isValid();
    virtual void accept();
    
public:
	explicit TimeSeriesDialog(QWidget *parent, const QList<TimeSeries*> &timeSeriesList, const TimeSeriesType &timeSeriesType);
    ~TimeSeriesDialog();
    
    QList<TimeSeries*> getTimeSeriesList() const;
    void setBoundaryCondition(BoundaryCondition *boundaryCondition);
    
private slots:
    void on_btnAddEntry_clicked();
    void on_btnImportCSV_clicked();
    void on_btnRemoveSelected_clicked();
    void on_btnClear_clicked();
};

#endif // TIME_SERIES_DIALOG_H