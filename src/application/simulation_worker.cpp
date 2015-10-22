#include "include/application/simulation_worker.h"
#include "include/application/simulation_progress_listener.h"
#include "include/application/simulation_recovery_listener.h"

extern "C" {
    void startSimulation(SimulationDataType::Simulation *simulation);
}

SimulationWorker::SimulationWorker(Simulation *simulation) : simulation(simulation) {
    this->moveToThread(&thread);
    thread.start();
}

SimulationWorker::~SimulationWorker() {
    thread.wait();
}

Simulation* SimulationWorker::getSimulation() const {
    return simulation;
}

void SimulationWorker::simulate() {
    if (simulation->getStatus() == SimulationStatus::IDLE || simulation->getStatus() == SimulationStatus::PAUSED) {
        SimulationRepository::updateSimulationStatus(simulation, SimulationStatus::RUNNING);
        SimulationDataType::Simulation *simulationStruct = simulation->toSimulationDataType();
        
        SimulationProgressListener progressListener;
        connect(this, SIGNAL(listenProgress(Simulation*)), &progressListener, SLOT(listen(Simulation*)));
        emit listenProgress(simulation);

        SimulationRecoveryListener recoveryListener;
        connect(this, SIGNAL(listenRecoveryVariables(Simulation*)), &recoveryListener, SLOT(listen(Simulation*)));
        emit listenRecoveryVariables(simulation);
        
        startSimulation(simulationStruct);
    }
}