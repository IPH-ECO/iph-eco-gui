#ifndef TIME_SERIES_CHART_DIALOG_H
#define TIME_SERIES_CHART_DIALOG_H

#include <domain/simulation.h>
#include <QAbstractButton>
#include <QFileInfoList>
#include <QWidget>
#include <QDialog>

namespace Ui {
	class TimeSeriesChartDialog;
}

class TimeSeriesChartDialog : public QDialog {
	Q_OBJECT
private:
	Ui::TimeSeriesChartDialog *ui;
    Simulation *simulation;
public:
	explicit TimeSeriesChartDialog(QWidget *parent, Simulation *simulation, const int &numberOfMapLayers);
    ~TimeSeriesChartDialog();
private slots:
    void on_btnBrowseShapefile_clicked();
    void on_btnPlot_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
};

#endif // TIME_SERIES_CHART_DIALOG_H