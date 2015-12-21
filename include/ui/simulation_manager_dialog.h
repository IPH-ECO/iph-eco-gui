#ifndef SIMULATION_MANAGER_DIALOG_H
#define SIMULATION_MANAGER_DIALOG_H

#include <domain/simulation.h>

#include <QList>
#include <QWidget>
#include <QDialog>
#include <QAction>
#include <QTableWidget>
#include <QTableWidgetItem>

namespace Ui {
	class SimulationManagerDialog;
}

class SimulationManagerDialog : public QDialog {
    Q_OBJECT
private:
	Ui::SimulationManagerDialog *ui;
    QList<QAction*> toolBarActions;
    
    Simulation* getCurrentSimulation() const;
    QTableWidget* getTableWidgetBySimulationStatus(const SimulationStatus &status);
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
    void on_btnClose_clicked();
};

#endif // SIMULATION_MANAGER_DIALOG_H