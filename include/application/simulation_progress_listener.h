#ifndef SIMULATION_PROGRESS_LISTENER_H
#define SIMULATION_PROGRESS_LISTENER_H

#include <repository/simulation_repository.h>

#include <QThread>

class SimulationProgressListener : public QObject {
    Q_OBJECT
private:
    QThread thread;
public:
    SimulationProgressListener() {
        this->moveToThread(&thread);
        thread.start();
    }
    
    ~SimulationProgressListener() {
        thread.wait();
    }
public slots:
    void listen(Simulation *simulation) {
		SimulationDataType::Simulation *simulationStruct = simulation->toSimulationDataType();
		int progress = simulationStruct->progress;

        while (simulation->getStatus() != SimulationStatus::FINISHED && simulation->getStatus() != SimulationStatus::ABORTED) {
            if (progress != simulationStruct->progress) {
				progress = simulationStruct->progress;
                SimulationRepository::updateSimulationProgress(simulation, progress);
			}
		}
    }
};

#endif // SIMULATION_PROGRESS_LISTENER_H