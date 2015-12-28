#ifndef TIME_SERIES_CHART_DIALOG_H
#define TIME_SERIES_CHART_DIALOG_H

#include <domain/simulation.h>
#include "simulation_vtk_widget.h"

#include <QWidget>
#include <QDialog>
#include <QSettings>
#include <vtkChartXY.h>
#include <QFileInfoList>
#include <QAbstractButton>
#include <vtkContextView.h>

namespace Ui {
	class TimeSeriesChartDialog;
}

class TimeSeriesChartDialog : public QDialog {
	Q_OBJECT
private:
    const QString BOUNDARY_DEFAULT_DIR_KEY;

    Ui::TimeSeriesChartDialog *ui;
    SimulationVTKWidget *simulationVTKWidget;
    vtkSmartPointer<vtkContextView> view;
    vtkSmartPointer<vtkChartXY> chart;
    QSettings *appSettings;
    
    bool isValid();
    QString getDefaultDirectory();
    vtkSmartPointer<vtkIdTypeArray> getCellsIds() const;
    virtual void reject();
public:
	explicit TimeSeriesChartDialog(QWidget *parent, SimulationVTKWidget *simulationVTKWidget);
    ~TimeSeriesChartDialog();
private slots:
    void on_btnBrowseShapefile_clicked();
    void on_btnPicker_toggled(bool checked);
    void on_btnClear_clicked();
    void on_btnPlot_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
    void btnExportToCSV_clicked();
};

#endif // TIME_SERIES_CHART_DIALOG_H