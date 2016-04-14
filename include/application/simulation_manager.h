#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

#include <domain/simulation.h>
#include <application/simulation_worker.h>

#include <QSet>

class SimulationManager : public QObject {
    Q_OBJECT
private:
	static SimulationManager *instance;
	QSet<SimulationWorker*> workers;

	SimulationManager();

	void createWorker(Simulation *simulation);
	SimulationWorker* getWorker(Simulation *simulation) const;
public:
	static SimulationManager* getInstance();

	~SimulationManager();

	void addIdle(Simulation *simulation);
	void start(Simulation *simulation);
	void pause(Simulation *simulation);
    void resume(Simulation *simulation);
	void finish(Simulation *simulation);
    void remove(Simulation *simulation);
signals:
    void simulationStarted();
};

#endif // SIMULATION_MANAGER_H