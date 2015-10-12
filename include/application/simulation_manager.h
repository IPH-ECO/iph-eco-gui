#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

#include "include/domain/simulation.h"
#include "include/application/simulation_worker.h"

#include <QSet>

class SimulationManager {
private:
	static SimulationManager *instance;
	QSet<SimulationWorker*> workers;

	SimulationManager();

	SimulationWorker* add(Simulation *simulation);
	void remove(Simulation *simulation);
	SimulationWorker* getWorker(Simulation *simulation) const;
public:
	static SimulationManager* getInstance();

	~SimulationManager();

	void addIdle(Simulation *simulation);
	void start(Simulation *simulation);
	void pause(Simulation *simulation);
	void stop(Simulation *simulation);
};

#endif // SIMULATION_MANAGER_H