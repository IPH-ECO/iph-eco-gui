#ifndef SIMULATION_REPOSITORY_H
#define SIMULATION_REPOSITORY_H

#include <domain/simulation.h>

#include <QTreeWidget>

class SimulationRepository {
public:
	static void loadOutputParametersTree(QTreeWidget *trOutputVariables, WaterQualityConfiguration *waterQualityConfiguration);
	static QMap<QString, QString> loadHydroOutputParametersLabels(const QStringList &parameters);
    static void updateSimulationStatus(Simulation *simulation, const SimulationStatus &status);
    static void updateSimulationProgress(Simulation *simulation, int progress);
    static void deleteSimulation(Simulation *simulation);
    static void saveRecoveryVariables(Simulation *simulation);
    static void appendChildren(WaterQualityParameter *parent, QTreeWidgetItem *parentItem, const QString &module);
};
#endif // SIMULATION_REPOSITORY_H
