#ifndef VIEW_RESULTS_DIALOG_H
#define VIEW_RESULTS_DIALOG_H

#include <domain/simulation.h>
#include <ui/abstract_mesh_dialog.h>

#include <QTimer>
#include <QMutex>
#include <QWidget>
#include <QToolButton>
#include <QTableWidgetItem>

namespace Ui {
	class ViewResultsDialog;
}

class ViewResultsDialog : public AbstractMeshDialog {
    Q_OBJECT
private:
	Ui::ViewResultsDialog *ui;
    Simulation *currentSimulation;
    QString axesScale;
    QTimer frameTimer;
    QMutex mutex;

    void fillLayersComboBox(Simulation *simulation);
    QString getLayerKeyFromButton(QToolButton *button) const;
public:
	explicit ViewResultsDialog(QWidget *parent);
    virtual void showEvent(QShowEvent *event);
private slots:
    void onUpdateSimulationProgress(int progress);
    void onUpdateSimulationStatus(SimulationStatus status);
    void onSimulationCreated(Simulation *simulation);
    void on_tblSimulations_cellClicked(int row, int column);
    void on_btnRefresh_clicked();
    void on_btnClose_clicked();
    void on_btnFirstFrame_clicked();
    void on_btnPreviousFrame_clicked();
    void on_btnStartReproduction_clicked();
    void on_btnPauseReproduction_clicked();
    void on_btnNextFrame_clicked();
    void on_btnLastFrame_clicked();
    void on_btnAddLayer_clicked();
    void on_spxFrame_valueChanged(int i);
    void toggleLayerVisibility(bool show);
    void showTimeSeriesChart();
    void removeLayer();
    void renderNextFrame();
    void editLayerProperties();
    void showAxesDialog();
    void showExportVideoDialog();
};

#endif // VIEW_RESULTS_DIALOG_H