#ifndef TIME_SERIES_DIALOG_H
#define TIME_SERIES_DIALOG_H

#include <QSettings>
#include <QDialog>
#include <QWidget>
#include <QMap>

namespace Ui {
	class TimeSeriesDialog;
}

class TimeSeriesDialog : public QDialog {
    Q_OBJECT
private:
    const QString HYDRODYNAMIC_DEFAULT_DIR_KEY;

    QMap<QString, double> timeSeries;
    QSettings *appSettings;
	Ui::TimeSeriesDialog *ui;
    
public:
	explicit TimeSeriesDialog(QWidget *parent);
    ~TimeSeriesDialog();
    
    QString getDefaultDirectory();
    QMap<QString, double> getTimeSeries() const;
    
private slots:
    void on_btnAddEntry_clicked();
    void on_btnImportCSV_clicked();
    void on_btnRemoveSelected_clicked();
    void on_btnClear_clicked();
};

#endif // TIME_SERIES_DIALOG_H