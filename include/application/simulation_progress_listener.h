#ifndef SIMULATION_PROGRESS_LISTENER_H
#define SIMULATION_PROGRESS_LISTENER_H

#include "include/domain/simulation.h"

#include <QThread>

class SimulationProgressListener : public QThread {
private:
	Simulation *simulation;

	virtual void run() {
		SimulationDataType::Simulation *simulationStruct = simulation->toSimulationDataType();
		int progress = simulationStruct->progress;

		while (progress != 100) {
			if (progress != simulationStruct->progress) {
				progress = simulationStruct->progress;
                simulation->setProgress(progress);
			}
		}
	}
public:
	SimulationProgressListener(Simulation *simulation) : simulation(simulation) {}
};

#endif // SIMULATION_PROGRESS_LISTENER_H