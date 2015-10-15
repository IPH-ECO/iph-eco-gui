#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

#include "include/domain/simulation.h"
#include "include/application/simulation_worker.h"

#include <QSet>

class SimulationManager : public QObject {
    Q_OBJECT
private:
	static SimulationManager *instance;
	QSet<SimulationWorker*> workers;

	SimulationManager();

	void createWorker(Simulation *simulation);
	void remove(Simulation *simulation);
	SimulationWorker* getWorker(Simulation *simulation) const;
public:
	static SimulationManager* getInstance();

	~SimulationManager();

	void addIdle(Simulation *simulation);
	void start(Simulation *simulation);
	void pause(Simulation *simulation);
    void resume(Simulation *simulation);
	void stop(Simulation *simulation);
signals:
    void startSimulation();
};

#endif // SIMULATION_MANAGER_H