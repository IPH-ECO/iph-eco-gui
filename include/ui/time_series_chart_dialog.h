#ifndef TIME_SERIES_CHART_DIALOG_H
#define TIME_SERIES_CHART_DIALOG_H

#include <domain/simulation.h>
#include "simulation_vtk_widget.h"
#include <domain/layer_properties.h>
#include <utility/time_series_chart_mouse_interactor.h>

#include <QWidget>
#include <QDialog>
#include <QSettings>
#include <vtkChartXY.h>
#include <vtkRenderer.h>
#include <QFileInfoList>
#include <QAbstractButton>
#include <vtkStringArray.h>
#include <vtkContextView.h>
#include <vtkCubeAxesActor.h>
#include <vtkRectilinearGrid.h>
#include <vtkColorTransferFunction.h>

namespace Ui {
	class TimeSeriesChartDialog;
}

class TimeSeriesChartDialog : public QDialog {
	Q_OBJECT
private:
    const QString TIME_SERIES_CHART_DIR_KEY;

    Ui::TimeSeriesChartDialog *ui;
    SimulationVTKWidget *simulationVTKWidget;
    TimeSeriesChartMouseInteractor *timeSeriesInteractor;
    vtkSmartPointer<vtkRectilinearGrid> rectilinearGrid;
    vtkSmartPointer<vtkCubeAxesActor> axesActor;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkContextView> view;
    vtkSmartPointer<vtkChartXY> chart;
    LayerProperties *layerProperties;
    QSettings *appSettings;
    QString layerKey;
    
    bool isValid();
    QString getDefaultDirectory();
    vtkSmartPointer<vtkIdTypeArray> getCellsIds() const;
    vtkIdType getCorrespondingCell(const vtkIdType &sourceCellId, const int &layer) const;
    void renderVerticalProfileGrid(vtkSmartPointer<vtkDoubleArray> x, vtkSmartPointer<vtkDoubleArray> y, vtkSmartPointer<vtkDoubleArray> scalars);
    void renderVerticalProfileAxes(double *xRange, double *yRange, vtkSmartPointer<vtkStringArray> timeStamps);
    vtkSmartPointer<vtkColorTransferFunction> buildColorTransferFunction(double *scalarBarRange);
    vtkSmartPointer<vtkDoubleArray> getGridArray(vtkSmartPointer<vtkUnstructuredGrid> sourceGrid) const;
    virtual void reject();
public:
	explicit TimeSeriesChartDialog(QWidget *parent, SimulationVTKWidget *simulationVTKWidget, const QString &layerKey);
    ~TimeSeriesChartDialog();
private slots:
    void on_btnBrowsePointsFile_clicked();
    void on_btnPicker_toggled(bool checked);
    void on_btnClear_clicked();
    void on_btnPlot_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_chkVerticalProfile_toggled(bool checked);
    void btnExportToCSV_clicked();
    void btnExportToPNG_clicked();
    void btnLayerProperties_clicked();
};

#endif // TIME_SERIES_CHART_DIALOG_H