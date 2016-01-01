#ifndef TIME_SERIES_CHART_DIALOG_H
#define TIME_SERIES_CHART_DIALOG_H

#include <domain/simulation.h>
#include "simulation_vtk_widget.h"
#include <utility/time_series_chart_mouse_interactor.h>

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
    TimeSeriesChartMouseInteractor *timeSeriesInteractor;
    vtkSmartPointer<vtkContextView> view;
    vtkSmartPointer<vtkChartXY> chart;
    QSettings *appSettings;
    
    bool isValid();
    QString getDefaultDirectory();
    vtkSmartPointer<vtkIdTypeArray> getCellsIds() const;
    vtkIdType getCorrespondingCell(const vtkIdType &sourceCellId) const;
    virtual void reject();
public:
	explicit TimeSeriesChartDialog(QWidget *parent, SimulationVTKWidget *simulationVTKWidget);
    ~TimeSeriesChartDialog();
private slots:
    void on_btnBrowseShapeFile_clicked();
    void on_btnPicker_toggled(bool checked);
    void on_btnClear_clicked();
    void on_btnPlot_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_chkVerticalProfile_toggled(bool checked);
    void btnExportToCSV_clicked();
    void btnExportToPNG_clicked();
};

#endif // TIME_SERIES_CHART_DIALOG_H