#ifndef SIMULATION_SERVICE_H
#define SIMULATION_SERVICE_H

#include "include/application/iph_types.h"
#include "include/domain/hydrodynamic_configuration.h"

class SimulationService {
private:
	static SimulationService *instance;

	SimulationService();
public:
	static SimulationService* getInstance();

	void run(HydrodynamicConfiguration *hydrodynamicConfiguration);
};

#endif // SIMULATION_SERVICE_H