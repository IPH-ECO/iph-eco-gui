#ifndef SIMULATION_MANAGER_DIALOG_H
#define SIMULATION_MANAGER_DIALOG_H

#include <QWidget>

namespace Ui {
	class SimulationManagerDialog;
}

class SimulationManagerDialog : public QWidget {
private:
	Ui::SimulationManagerDialog *ui;
public:
	explicit SimulationManagerDialog(QWidget *parent);
};

#endif // SIMULATION_MANAGER_DIALOG_H