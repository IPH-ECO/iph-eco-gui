#ifndef TIME_SERIES_CHART_DIALOG_H
#define TIME_SERIES_CHART_DIALOG_H

#include <domain/simulation.h>
#include "simulation_vtk_widget.h"

#include <QWidget>
#include <QDialog>
#include <QFileInfoList>
#include <vtkContextView.h>
#include <QAbstractButton>

namespace Ui {
	class TimeSeriesChartDialog;
}

class TimeSeriesChartDialog : public QDialog {
	Q_OBJECT
private:
    Ui::TimeSeriesChartDialog *ui;
    SimulationVTKWidget *simulationVTKWidget;
    vtkSmartPointer<vtkContextView> view;
    
    virtual void reject();
public:
	explicit TimeSeriesChartDialog(QWidget *parent, SimulationVTKWidget *simulationVTKWidget);
    ~TimeSeriesChartDialog();
private slots:
    void on_btnBrowseShapefile_clicked();
    void on_btnPlot_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
    void btnExportToCSV_clicked();
};

#endif // TIME_SERIES_CHART_DIALOG_H