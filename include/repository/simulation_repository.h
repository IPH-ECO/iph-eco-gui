#ifndef SIMULATION_REPOSITORY_H
#define SIMULATION_REPOSITORY_H

#include <QTreeWidget>

class SimulationRepository {
public:
	static void buildTree(QTreeWidget *trOutputVariables);
};

#endif // SIMULATION_REPOSITORY_H