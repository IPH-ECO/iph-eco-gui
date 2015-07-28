#ifndef TIME_SERIES_DIALOG_H
#define TIME_SERIES_DIALOG_H

#include <QDialog>
#include <QWidget>
#include <QSettings>

namespace Ui {
	class TimeSeriesDialog;
}

class TimeSeriesDialog : public QDialog {
    Q_OBJECT
private:
    const QString HYDRODYNAMIC_DEFAULT_DIR_KEY;

    QSettings *appSettings;
	Ui::TimeSeriesDialog *ui;
    
public:
	explicit TimeSeriesDialog(QWidget *parent);
    ~TimeSeriesDialog();
    
    QString getDefaultDirectory();
    
private slots:
    void on_btnImportCSV_clicked();
    void on_btnClear_clicked();
};

#endif // TIME_SERIES_DIALOG_H