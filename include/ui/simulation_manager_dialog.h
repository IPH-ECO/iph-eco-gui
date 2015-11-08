#ifndef SIMULATION_MANAGER_DIALOG_H
#define SIMULATION_MANAGER_DIALOG_H

#include "include/domain/simulation.h"
#include "include/ui/abstract_mesh_dialog.h"

#include <QList>
#include <QTimer>
#include <QWidget>
#include <QDialog>
#include <QAction>
#include <QShowEvent>
#include <QTableWidget>
#include <QTableWidgetItem>

namespace Ui {
	class SimulationManagerDialog;
}

class SimulationManagerDialog : public AbstractMeshDialog {
    Q_OBJECT
private:
	Ui::SimulationManagerDialog *ui;
    QList<QAction*> toolBarActions;
    QTimer frameTimer;
    
    Simulation* getCurrentSimulation() const;
    QTableWidget* getTableWidgetBySimulationStatus(const SimulationStatus &status);
    void fillLayersComboBox(Simulation *simulation);
public:
	explicit SimulationManagerDialog(QWidget *parent);
private slots:
    void on_tblAll_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_tblIdle_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_tblRunning_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_tblPaused_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_tblFinished_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void onUpdateSimulationProgress(int progress);
    void onUpdateSimulationStatus(SimulationStatus status);
    void onSimulationCreated(Simulation *simulation);
    void on_btnResume_clicked();
    void on_btnPause_clicked();
    void on_btnFinish_clicked();
    void on_btnRemove_clicked();
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
};

#endif // SIMULATION_MANAGER_DIALOG_H