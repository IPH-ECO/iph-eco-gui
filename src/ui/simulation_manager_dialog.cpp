#include "include/ui/simulation_manager_dialog.h"
#include "ui_simulation_manager_dialog.h"

SimulationManagerDialog::SimulationManagerDialog(QWidget *parent) : QWidget(parent), ui(new Ui::SimulationManagerDialog) {
	ui->setupUi(this);
}