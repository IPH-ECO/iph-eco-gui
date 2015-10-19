#ifndef SIMULATION_H
#define SIMULATION_H

#include "hydrodynamic_configuration.h"
#include  "meteorological_configuration.h"
#include "simulation_data_type.h"

#include <QDateTime>
#include <QString>
#include <QList>
#include <QMap>

enum class SimulationType { DEFAULT = 1, PARAMETER_CALIBRATION, SENSIBILITY_ANALYSIS };
enum class SimulationStatus {
	IDLE = 1, // Never entered on RUNNING state
	RUNNING, // In execution
	PAUSED, // Paused by either the user or the application
	FINISHED, // The execution terminated sucessfuly
    ABORTED, // The user aborted (removed) the simulation
    UNDEFINED
};

Q_DECLARE_METATYPE(SimulationStatus)

class Simulation : public QObject {
	Q_OBJECT
private:
	uint id;
	QString label;
	SimulationType simulationType;
    uint startTime;
	uint initialTime;
	double period;
	int stepTime;
	QList<double> layers;
	HydrodynamicConfiguration *hydrodynamicConfiguration;
	// WaterQualityConfiguration *waterQualityConfiguration;
    MeteorologicalConfiguration *meteorologicalConfiguration;
    double minimumVerticalLimit;
    double maximumVerticalLimit;
	QString observations;
    QString outputDirectory;
    QStringList outputParameters;
    SimulationStatus status;
    QString recoveryVariables;
    int outputTimeInterval;
    int autosaveTimeInterval;
    int progress;
    
	// Transient attributes
	SimulationDataType::Simulation *simulationStruct;
	bool startOnCreate;
	SimulationStatus previousStatus;
    
    void loadRecoveryVariables();
	static QMap<SimulationType, QString> simulationTypesMap;
	static QMap<SimulationStatus, QString> simulationStatusMap;
public:
    Simulation();
    ~Simulation();
	uint getId() const;
	void setId(uint id);
	bool isPersisted() const;
	QString getLabel() const;
	void setLabel(const QString &label);
	SimulationType getSimulationType() const;
	void setSimulationType(const SimulationType &simulationType);
    uint getStartTime() const;
    QDateTime getStartTimeAsDateTime() const;
    void setStartTime(uint startTime);
	uint getInitialTime() const;
    QDateTime getInitialTimeAsDateTime() const;
	void setInitialTime(uint initialTime);
	double getPeriod() const;
	void setPeriod(double period);
	int getStepTime() const;
	void setStepTime(int stepTime);
	QList<double> getLayers() const;
    QString getLayersAsString() const;
	void setLayers(const QList<double> &layers);
    void setLayersFromString(const QString &layersStr);
	bool addLayer(int layer);
	void removeLayer(int index);
	HydrodynamicConfiguration* getHydrodynamicConfiguration() const;
	void setHydrodynamicConfiguration(HydrodynamicConfiguration *configuration);
    MeteorologicalConfiguration* getMeteorologicalConfiguration() const;
    void setMeteorologicalConfiguration(MeteorologicalConfiguration *configuration);
    double getMinimumVerticalLimit() const;
    void setMinimumVerticalLimit(double minimumVerticalLimit);
    double getMaximumVerticalLimit() const;
    void setMaximumVerticalLimit(double maximumVerticalLimit);
	QString getObservations() const;
	void setObservation(const QString &observations);
	bool getStartOnCreate() const;
	void setStartOnCreate(bool startOnCreate);
    int getOutputTimeInterval() const;
    void setOutputTimeInterval(int outputTimeInterval);
    int getAutosaveTimeInterval() const;
    void setAutosaveTimeInterval(int autosaveTimeInterval);
    QString getOutputDirectory() const;
    void setOutputDirectory(const QString &outputDirectory);
    QStringList getOutputParameters() const;
    void setOutputParameters(const QStringList &outputParameters);
    SimulationStatus getStatus() const;
    void setStatus(const SimulationStatus &status);
    QString getRecoveryVariables() const;
    void setRecoveryVariables(const QString &recoveryVariables);
    void buildRecoveryVariablesJson();
    SimulationStatus getPreviousStatus() const;
    int getProgress() const;
    void setProgress(int progress);
    SimulationDataType::Simulation* toSimulationDataType();

	static QMap<SimulationType, QString> getSimulationTypesMap();
	static QMap<SimulationStatus, QString> getSimulationStatusMap();

signals:
	void updateProgress(int progress);
    void updateStatus(SimulationStatus status);
};

#endif // SIMULATION_H