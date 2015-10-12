#ifndef SIMULATION_MANAGER_DIALOG_H
#define SIMULATION_MANAGER_DIALOG_H

#include <QWidget>
#include <QAction>
#include <QList>

namespace Ui {
	class SimulationManagerDialog;
}

class SimulationManagerDialog : public QWidget {
    Q_OBJECT
private:
	Ui::SimulationManagerDialog *ui;
    QList<QAction*> toolBarActions;
public:
	explicit SimulationManagerDialog(QWidget *parent);
private slots:
    void on_btnClose_clicked();
};

#endif // SIMULATION_MANAGER_DIALOG_H