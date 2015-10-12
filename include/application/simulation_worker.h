#ifndef SIMULATION_WORKER_H
#define SIMULATION_WORKER_H

#include "include/domain/simulation.h"

#include <QThread>

class SimulationWorker : public QThread {
private:
	Simulation *simulation;

	virtual void run();
public:
	SimulationWorker(Simulation *simulation);
    Simulation* getSimulation() const;
};

#endif // SIMULATION_WORKER_H