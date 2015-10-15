#include "include/application/simulation_manager.h"
#include "include/repository/simulation_repository.h"

#include <QThread>

SimulationManager* SimulationManager::instance = nullptr;

SimulationManager::SimulationManager() {}

SimulationManager::~SimulationManager() {
	for (SimulationWorker *worker : workers) {
		delete worker;
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
    
    connect(this, SIGNAL(startSimulation()), worker, SLOT(simulate()));
    workers.insert(worker);
}

void SimulationManager::remove(Simulation *simulation) {
	for (SimulationWorker *worker : workers) {
		if (worker->getSimulation() == simulation) {
			delete worker;
			workers.remove(worker);
		}
	}
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
	this->createWorker(simulation);
}

void SimulationManager::start(Simulation *simulation) {
	this->createWorker(simulation);
    emit startSimulation();
}

void SimulationManager::pause(Simulation *simulation) {
    SimulationRepository::updateSimulationStatus(simulation, SimulationStatus::PAUSED);
    SimulationWorker *worker = this->getWorker(simulation);
    worker->pause();
}

void SimulationManager::resume(Simulation *simulation) {
    SimulationWorker *worker = this->getWorker(simulation);
    
    if (worker) {
        worker->resume();
    } else {
        this->start(simulation);
    }
}

void SimulationManager::stop(Simulation *simulation) {
	this->remove(simulation);
}