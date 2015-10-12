#ifndef SIMULATION_REPOSITORY_H
#define SIMULATION_REPOSITORY_H

#include "include/domain/simulation.h"

#include <QTreeWidget>

class SimulationRepository {
public:
	static void loadOutputParametersTree(QTreeWidget *trOutputVariables);
    static void updateSimulationStatus(Simulation *simulation, const SimulationStatus &status);
};

#endif // SIMULATION_REPOSITORY_H