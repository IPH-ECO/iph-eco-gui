#include "include/application/simulation_worker.h"
#include "include/application/simulation_progress_listener.h"

extern "C" {
    void startSimulation(SimulationDataType::Simulation *simulation);
}

SimulationWorker::SimulationWorker(Simulation *simulation) : simulation(simulation) {}

Simulation* SimulationWorker::getSimulation() const {
    return simulation;
}

void SimulationWorker::run() {
	SimulationDataType::Simulation *simulationStruct = simulation->toSimulationDataType();
	SimulationProgressListener listener(simulation);

    listener.start();
    startSimulation(simulationStruct);
    listener.wait();
}