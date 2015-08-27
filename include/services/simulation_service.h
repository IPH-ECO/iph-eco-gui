#ifndef SIMULATION_SERVICE_H
#define SIMULATION_SERVICE_H

#include "include/domain/simulation.h"

class SimulationService {
private:
	static SimulationService *instance;

	SimulationService();
public:
	static SimulationService* getInstance();

	void run(Simulation *simulation);
};

#endif // SIMULATION_SERVICE_H