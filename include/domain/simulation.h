#ifndef SIMULATION_H
#define SIMULATION_H

#include "hydrodynamic_configuration.h"
#include "simulation_data_type.h"

#include <QString>
#include <QList>
#include <QMap>

enum class SimulationType { DEFAULT = 1, PARAMETER_CALIBRATION, SENSIBILITY_ANALYSIS };

class Simulation {
private:
	uint id;
	QString label;
	SimulationType simulationType;
	uint initialTime;
	double period;
	int stepTime;
	QList<double> layers;
	HydrodynamicConfiguration *hydrodynamicConfiguration;
	// WaterQualityConfiguration *waterQualityConfiguration;
	// MeteorologicalConfiguration *meteorologicalConfiguration;
    double minimumVerticalLimit;
    double maximumVerticalLimit;
	QString observations;

	// Transient attributes
	bool startOnCreate;

	static QMap<SimulationType, QString> simulationTypesMap;
public:
    Simulation();
	uint getId() const;
	void setId(uint id);
	bool isPersisted() const;
	QString getLabel() const;
	void setLabel(const QString &label);
	SimulationType getSimulationType() const;
	void setSimulationType(const SimulationType &simulationType);
	uint getInitialTime() const;
	void setInitialTime(uint initialTime);
	double getPeriod() const;
	void setPeriod(double period);
	int getStepTime() const;
	void setStepTime(int stepTime);
	QList<double> getLayers() const;
	void setLayers(const QList<double> &layers);
	bool addLayer(int layer);
	void removeLayer(int index);
	HydrodynamicConfiguration* getHydrodynamicConfiguration() const;
	void setHydrodynamicConfiguration(HydrodynamicConfiguration *configuration);
    double getMinimumVerticalLimit() const;
    void setMinimumVerticalLimit(double minimumVerticalLimit);
    double getMaximumVerticalLimit() const;
    void setMaximumVerticalLimit(double maximumVerticalLimit);
	QString getObservations() const;
	void setObservation(const QString &observations);
	bool getStartOnCreate() const;
	void setStartOnCreate(bool startOnCreate);
	SimulationDataType::Simulation toSimulationDataType() const;

	static QMap<SimulationType, QString> getSimulationTypesMap();
};

#endif // SIMULATION_H