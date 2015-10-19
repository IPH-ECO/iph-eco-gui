#ifndef SIMULATION_WORKER_H
#define SIMULATION_WORKER_H

#include "include/domain/simulation.h"

#include <QThread>
#include <QTimer>

class SimulationWorker : public QObject {
    Q_OBJECT
private:
    Simulation *simulation;
    QThread thread;
    QTimer timer;
public slots:
    void simulate();
private slots:
    void autosave();
public:
	SimulationWorker(Simulation *simulation);
    ~SimulationWorker();
    Simulation* getSimulation() const;
signals:
    void listenProgress(Simulation *simulation);
};

#endif // SIMULATION_WORKER_H