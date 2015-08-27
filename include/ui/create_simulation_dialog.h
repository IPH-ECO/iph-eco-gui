#ifndef CREATE_SIMULATION_DIALOG_H
#define CREATE_SIMULATION_DIALOG_H

#include "include/domain/simulation.h"

#include <QDialog>
#include <QWidget>

namespace Ui {
	class CreateSimulationDialog;
}

class CreateSimulationDialog : public QDialog {
	Q_OBJECT
private:
	Ui::CreateSimulationDialog *ui;
    Simulation *unsavedSimulation;
	Simulation *currentSimulation;

	bool isValid();

public:
	explicit CreateSimulationDialog(QWidget *parent = 0);
    ~CreateSimulationDialog();
};

#endif // CREATE_SIMULATION_DIALOG_H