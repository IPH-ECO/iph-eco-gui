#include "include/services/simulation_service.h"

extern "C" {
    void startSimulation(SimulationDataType::Simulation *simulation);
}

SimulationService* SimulationService::instance = nullptr;

SimulationService::SimulationService() {}

SimulationService* SimulationService::getInstance() {
	if (!instance) {
		instance = new SimulationService();
	}

	return instance;
}

void SimulationService::run(Simulation *simulation) {
    SimulationDataType::Simulation sSimulation = simulation->toSimulationDataType();
    
    startSimulation(&sSimulation);
}
