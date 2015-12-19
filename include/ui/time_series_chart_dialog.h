#ifndef TIME_SERIES_CHART_DIALOG_H
#define TIME_SERIES_CHART_DIALOG_H

#include <QWidget>
#include <QDialog>

namespace Ui {
	class TimeSeriesChartDialog;
}

class TimeSeriesChartDialog : public QDialog {
	Q_OBJECT
private:
	Ui::TimeSeriesChartDialog *ui;
public:
	explicit TimeSeriesChartDialog(QWidget *parent);
    ~TimeSeriesChartDialog();
};

#endif // TIME_SERIES_CHART_DIALOG_H