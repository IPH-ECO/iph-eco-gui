#include "include/application/simulation_worker.h"
#include "include/application/simulation_progress_listener.h"

extern "C" {
    void startSimulation(SimulationDataType::Simulation *simulation);
}

SimulationWorker::SimulationWorker(Simulation *simulation) : simulation(simulation) {
    this->moveToThread(&thread);
    thread.start();
    
    if (simulation->getAutosaveTimeInterval() > 0) {
        connect(&timer, SIGNAL(timeout()), this, SLOT(autosave()));
        timer.setInterval(simulation->getAutosaveTimeInterval() * simulation->getStepTime() * 1000);
        timer.start();
    }
}

SimulationWorker::~SimulationWorker() {
    thread.wait();
}

Simulation* SimulationWorker::getSimulation() const {
    return simulation;
}

void SimulationWorker::simulate() {
    if (simulation->getStatus() == SimulationStatus::IDLE || simulation->getStatus() == SimulationStatus::PAUSED) {
        SimulationDataType::Simulation *simulationStruct = simulation->toSimulationDataType();
        SimulationProgressListener progressListener;
        
        SimulationRepository::updateSimulationStatus(simulation, SimulationStatus::RUNNING);
        connect(this, SIGNAL(listenProgress(Simulation*)), &progressListener, SLOT(listen(Simulation*)));
        emit listenProgress(simulation);
        startSimulation(simulationStruct);
    }
}

void SimulationWorker::autosave() {
    if (simulation->getStatus() == SimulationStatus::RUNNING) {
        SimulationRepository::saveRecoveryVariables(simulation);
    }
}