#include "include/application/simulation_manager.h"

#include "include/repository/simulation_repository.h"

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

SimulationWorker* SimulationManager::add(Simulation *simulation) {
	for (SimulationWorker *worker : workers) {
		if (worker->getSimulation() == simulation) {
			return worker;
		}
	}

	SimulationWorker *worker = new SimulationWorker(simulation);

	workers.insert(worker);

	return worker;
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
	this->add(simulation);
}

void SimulationManager::start(Simulation *simulation) {
	SimulationRepository::updateSimulationStatus(simulation, SimulationStatus::RUNNING);
	SimulationWorker *worker = this->add(simulation);
	worker->start();
}

void SimulationManager::pause(Simulation *simulation) {
    SimulationRepository::updateSimulationStatus(simulation, SimulationStatus::PAUSED);
    SimulationWorker *worker = this->getWorker(simulation);
}

void SimulationManager::stop(Simulation *simulation) {
	this->remove(simulation);
}