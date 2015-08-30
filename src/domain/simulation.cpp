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

SimulationDataType::Simulation Simulation::toSimulationDataType() const {
	SimulationDataType::Simulation simulation;
    Project *project = IPHApplication::getCurrentProject();
	std::string labelStr = this->label.toStdString();
	int i = 0;
    
    simulation.modules = 0;
    simulation.modules |= project->getHydrodynamic() ? 0x01 : 0;
    simulation.modules |= project->getWaterQuality() ? 0x02 : 0;
    simulation.modules |= project->getSediment() ? 0x04 : 0;

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

	return simulation;
}

QMap<SimulationType, QString> Simulation::getSimulationTypesMap() {
	return simulationTypesMap;
}
