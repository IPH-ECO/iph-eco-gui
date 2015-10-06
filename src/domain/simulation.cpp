#include "include/domain/simulation.h"
#include "include/application/iph_application.h"

Simulation::Simulation() : id(0), hydrodynamicConfiguration(nullptr) {}

QMap<SimulationType, QString> Simulation::simulationTypesMap = {
	std::pair<SimulationType, QString>(SimulationType::DEFAULT, "Default"),
	std::pair<SimulationType, QString>(SimulationType::PARAMETER_CALIBRATION, "Parameter calibration"),
	std::pair<SimulationType, QString>(SimulationType::SENSIBILITY_ANALYSIS, "Sensibility analysis")
};

uint Simulation::getId() const {
	return id;
}

void Simulation::setId(uint id) {
	if (!isPersisted()) {
        this->id = id;
    }
}

bool Simulation::isPersisted() const {
	return id != 0;
}

QString Simulation::getLabel() const {
	return label;
}

void Simulation::setLabel(const QString &label) {
	this->label = label;
}

SimulationType Simulation::getSimulationType() const {
	return simulationType;
}

void Simulation::setSimulationType(const SimulationType &simulationType) {
	this->simulationType = simulationType;
}

uint Simulation::getInitialTime() const {
	return initialTime;
}

void Simulation::setInitialTime(uint initialTime) {
	this->initialTime = initialTime;
}

double Simulation::getPeriod() const {
	return period;
}

void Simulation::setPeriod(double period) {
	this->period = period;
}

int Simulation::getStepTime() const {
	return stepTime;
}

void Simulation::setStepTime(int stepTime) {
	this->stepTime = stepTime;
}

QList<double> Simulation::getLayers() const {
	return layers;
}

void Simulation::setLayers(const QList<double> &layers) {
	this->layers = layers;
}

bool Simulation::addLayer(int layer) {
	if (layers.contains(layer)) {
		return false;
	}

	layers.append(layer);

	return true;
}

void Simulation::removeLayer(int index) {
	layers.removeAt(index);
}

HydrodynamicConfiguration* Simulation::getHydrodynamicConfiguration() const {
	return hydrodynamicConfiguration;
}

void Simulation::setHydrodynamicConfiguration(HydrodynamicConfiguration *configuration) {
	this->hydrodynamicConfiguration = configuration;
}

MeteorologicalConfiguration* Simulation::getMeteorologicalConfiguration() const {
    return meteorologicalConfiguration;
}

void Simulation::setMeteorologicalConfiguration(MeteorologicalConfiguration *configuration) {
    this->meteorologicalConfiguration = configuration;
}

double Simulation::getMinimumVerticalLimit() const {
    return minimumVerticalLimit;
}

void Simulation::setMinimumVerticalLimit(double minimumVerticalLimit) {
    this->minimumVerticalLimit = minimumVerticalLimit;
}

double Simulation::getMaximumVerticalLimit() const {
    return maximumVerticalLimit;
}

void Simulation::setMaximumVerticalLimit(double maximumVerticalLimit) {
    this->maximumVerticalLimit = maximumVerticalLimit;
}

QString Simulation::getObservations() const {
	return observations;
}

void Simulation::setObservation(const QString &observations) {
	this->observations = observations;
}

bool Simulation::getStartOnCreate() const {
	return startOnCreate;
}

void Simulation::setStartOnCreate(bool startOnCreate) {
	this->startOnCreate = startOnCreate;
}

QString Simulation::getOutputDirectory() const {
    return outputDirectory;
}

void Simulation::setOutputDirectory(const QString &outputDirectory) {
    this->outputDirectory = outputDirectory;
}

SimulationDataType::Simulation Simulation::toSimulationDataType() const {
	SimulationDataType::Simulation simulation;
    Project *project = IPHApplication::getCurrentProject();
	std::string labelStr = this->label.toStdString();
	std::string outputDirectoryStr = this->outputDirectory.toStdString();
	int i = 0;
    
    simulation.hydrodynamic = project->getHydrodynamic();
    simulation.waterQuality = project->getWaterQuality();
    simulation.sediment = project->getSediment();

    simulation.labelLength = this->label.size();
	simulation.label = new char[simulation.labelLength];
	strncpy(simulation.label, labelStr.c_str(), simulation.labelLength);
	simulation.simulationType = (int) this->simulationType;
	simulation.initialTime = this->initialTime;
	simulation.period = this->period;
	simulation.stepTime = this->stepTime;
	simulation.layersLength = this->layers.size();
	simulation.layers = new double[simulation.layersLength];

	for (double depth : layers) {
		simulation.layers[i++] = depth;
	}

	simulation.hydrodynamicConfiguration = this->hydrodynamicConfiguration->toSimulationDataType();
    simulation.meteorologicalConfiguration = this->meteorologicalConfiguration->toSimulationDataType();
    simulation.minimumVerticalLimit = this->minimumVerticalLimit;
    simulation.maximumVerticalLimit = this->maximumVerticalLimit;
    simulation.outputDirectoryLength = this->outputDirectory.size();
	simulation.outputDirectory = new char[simulation.outputDirectoryLength];
	strncpy(simulation.outputDirectory, outputDirectoryStr.c_str(), simulation.outputDirectoryLength);

	return simulation;
}

QMap<SimulationType, QString> Simulation::getSimulationTypesMap() {
	return simulationTypesMap;
}
