#ifndef SIMULATION_WORKER_H
#define SIMULATION_WORKER_H

#include <domain/simulation.h>

#include <QThread>

class SimulationWorker : public QObject {
    Q_OBJECT
private:
    Simulation *simulation;
    QThread thread;
public slots:
    void simulate();
public:
	SimulationWorker(Simulation *simulation);
    ~SimulationWorker();
    Simulation* getSimulation() const;
signals:
    void listenProgress(Simulation *simulation);
    void listenRecoveryVariables(Simulation *simulation);
};

#endif // SIMULATION_WORKER_H