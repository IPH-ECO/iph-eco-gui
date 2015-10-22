#ifndef SIMULATION_RECOVERY_LISTENER_H
#define SIMULATION_RECOVERY_LISTENER_H

#include "include/domain/simulation.h"

#include <QThread>

class SimulationRecoveryListener : public QObject {
	Q_OBJECT
private:
	QThread thread;
public:
	SimulationRecoveryListener() {
		this->moveToThread(&thread);
		thread.start();
	}

	~SimulationRecoveryListener() {
		thread.wait();
	}
public slots:
	void listen(Simulation *simulation) {
		SimulationDataType::Simulation *simulationStruct = simulation->toSimulationDataType();
		
        while (simulation->getStatus() == SimulationStatus::RUNNING) {
            SimulationDataType::RecoveryVariables *variables = simulationStruct->recoveryVariables;
            
			if (variables && variables->changed) {
				SimulationRepository::saveRecoveryVariables(simulation);
				variables->changed = false;
			}
		}
	}
};

#endif // SIMULATION_RECOVERY_LISTENER_H