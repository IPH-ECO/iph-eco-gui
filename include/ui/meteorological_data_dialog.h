#ifndef METEOROLOGICAL_DATA_DIALOG_H
#define METEOROLOGICAL_DATA_DIALOG_H

#include "include/domain/meteorological_station.h"
#include "include/domain/meteorological_configuration.h"

#include <QDialog>
#include <QWidget>

namespace Ui {
	class MeteorologicalDataDialog;
}

class MeteorologicalDataDialog : public QDialog {
	Q_OBJECT
public:
	explicit MeteorologicalDataDialog(QWidget *parent = 0);
    
    void toggleModelingOption(bool enable);
private slots:
    void on_cbxGridData_currentIndexChanged(const QString &name);
    void on_btnAddStation_clicked();
    void on_btnRemoveStation_clicked();
    void on_cbxFunction_currentIndexChanged(const QString &function);
    void on_rdoUtm_clicked(bool checked);
    void on_rdoLatLong_clicked(bool checked);
    void on_btnApplyStation_clicked();
    void on_btnApplyParameter_clicked();
    void on_btnClose_clicked();
    void on_trStations_itemSelectionChanged();
    void on_cbxType_currentIndexChanged(const QString &type);
    void on_btnShowTimeSeries_clicked();
private:
	Ui::MeteorologicalDataDialog *ui;
    MeteorologicalConfiguration *unsavedConfiguration;
    MeteorologicalConfiguration *currentConfiguration;
    MeteorologicalStation *currentStation;
};

#endif // METEOROLOGICAL_DATA_DIALOG_H