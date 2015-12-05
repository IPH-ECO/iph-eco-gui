#ifndef VIEW_RESULTS_DIALOG_H
#define VIEW_RESULTS_DIALOG_H

#include "include/domain/simulation.h"
#include "include/ui/abstract_mesh_dialog.h"

#include <QTimer>
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
    QTimer frameTimer;

    void fillLayersComboBox();
    QString getLayerKeyFromButton(QToolButton *button);
public:
	explicit ViewResultsDialog(QWidget *parent);
private slots:
    void onUpdateSimulationProgress(int progress);
    void onUpdateSimulationStatus(SimulationStatus status);
    void onSimulationCreated(Simulation *simulation);
    void on_tblSimulations_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
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
    void removeLayer();
    void renderNextFrame();
    void editLayerProperties();
};

#endif // VIEW_RESULTS_DIALOG_H