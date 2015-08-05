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

    QSettings *appSettings;
	Ui::TimeSeriesDialog *ui;
    BoundaryCondition *boundaryCondition;
    
    QString getDefaultDirectory();
    bool isValid();
    virtual void accept();
    
public:
	explicit TimeSeriesDialog(QWidget *parent, BoundaryCondition *boundaryCondition);
    ~TimeSeriesDialog();
    
private slots:
    void on_btnAddEntry_clicked();
    void on_btnImportCSV_clicked();
    void on_btnRemoveSelected_clicked();
    void on_btnClear_clicked();
};

#endif // TIME_SERIES_DIALOG_H