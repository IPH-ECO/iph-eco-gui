#include "include/application/simulation_worker.h"

extern "C" {
    void startSimulation(SimulationDataType::Simulation *simulation);
}

SimulationWorker::SimulationWorker(Simulation *simulation) : simulation(simulation) {}

Simulation* SimulationWorker::getSimulation() const {
    return simulation;
}

void SimulationWorker::run() {
	SimulationDataType::Simulation sSimulation = simulation->toSimulationDataType();

	std::cout << "Running simulation #" << simulation->getId() << std::endl;
    startSimulation(&sSimulation);
}