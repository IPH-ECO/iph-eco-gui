#include "include/application/simulation_manager.h"
#include "include/repository/simulation_repository.h"

#include <QThread>

SimulationManager* SimulationManager::instance = nullptr;

SimulationManager::SimulationManager() {}

SimulationManager::~SimulationManager() {
	for (SimulationWorker *worker : workers) {
		worker->deleteLater();
	}
}

SimulationManager* SimulationManager::getInstance() {
	if (!instance) {
		instance = new SimulationManager();
	}

	return instance;
}

void SimulationManager::createWorker(Simulation *simulation) {
	SimulationWorker *worker = new SimulationWorker(simulation);
    
    connect(this, SIGNAL(simulationStarted()), worker, SLOT(simulate()));
    workers.insert(worker);
}

void SimulationManager::remove(Simulation *simulation) {
	for (SimulationWorker *worker : workers) {
		if (worker->getSimulation() == simulation) {
            workers.remove(worker);
            worker->deleteLater();
            break;
		}
	}
    
    simulation->setStatus(SimulationStatus::ABORTED);
    SimulationRepository::deleteSimulation(simulation);
}

SimulationWorker* SimulationManager::getWorker(Simulation *simulation) const {
	for (SimulationWorker *worker : workers) {
		if (worker->getSimulation() == simulation) {
			return worker;
		}
	}

	return nullptr;
}

void SimulationManager::addIdle(Simulation *simulation) {
    SimulationRepository::updateSimulationStatus(simulation, SimulationStatus::IDLE);
}

void SimulationManager::start(Simulation *simulation) {
    this->createWorker(simulation);
    emit simulationStarted();
}

void SimulationManager::pause(Simulation *simulation) {
    SimulationRepository::updateSimulationStatus(simulation, SimulationStatus::PAUSED);
}

void SimulationManager::resume(Simulation *simulation) {
    SimulationWorker *worker = this->getWorker(simulation);
    
    if (worker) {
        SimulationRepository::updateSimulationStatus(simulation, SimulationStatus::RUNNING);
    } else {
        this->start(simulation);
    }
}

void SimulationManager::finish(Simulation *simulation) {
    SimulationWorker *worker = this->getWorker(simulation);
    SimulationRepository::updateSimulationStatus(simulation, SimulationStatus::FINISHED);
    
    workers.remove(worker);
    worker->deleteLater();
}