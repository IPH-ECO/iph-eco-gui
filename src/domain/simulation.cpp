#include "include/domain/simulation.h"
#include "include/application/iph_application.h"

#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

Simulation::Simulation() :
    id(0), hydrodynamicConfiguration(nullptr), status(SimulationStatus::IDLE), progress(0), simulationStruct(nullptr), previousStatus(SimulationStatus::UNDEFINED)
{
    qRegisterMetaType<SimulationStatus>("SimulationStatus");
}

Simulation::~Simulation() {
	// TODO: delete children
    if (simulationStruct) {
        delete simulationStruct->recoveryVariables;
    }
    
	delete simulationStruct;
}

QMap<SimulationType, QString> Simulation::simulationTypesMap = {
	std::pair<SimulationType, QString>(SimulationType::DEFAULT, "Default"),
	std::pair<SimulationType, QString>(SimulationType::PARAMETER_CALIBRATION, "Parameter calibration"),
	std::pair<SimulationType, QString>(SimulationType::SENSIBILITY_ANALYSIS, "Sensibility analysis")
};

QMap<SimulationStatus, QString> Simulation::simulationStatusMap = {
	std::pair<SimulationStatus, QString>(SimulationStatus::IDLE, "Idle"),
	std::pair<SimulationStatus, QString>(SimulationStatus::RUNNING, "Running"),
	std::pair<SimulationStatus, QString>(SimulationStatus::PAUSED, "Paused"),
	std::pair<SimulationStatus, QString>(SimulationStatus::FINISHED, "Finished"),
    std::pair<SimulationStatus, QString>(SimulationStatus::ABORTED, "Aborted"),
    std::pair<SimulationStatus, QString>(SimulationStatus::UNDEFINED, "Undefined")
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

uint Simulation::getStartTime() const {
    return startTime;
}

QDateTime Simulation::getStartTimeAsDateTime() const {
    return QDateTime::fromTime_t(startTime, Qt::UTC);
}

void Simulation::setStartTime(uint startTime) {
    this->startTime = startTime;
}

uint Simulation::getInitialTime() const {
	return initialTime;
}

QDateTime Simulation::getInitialTimeAsDateTime() const {
    return QDateTime::fromTime_t(initialTime, Qt::UTC);
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

QString Simulation::getLayersAsString() const {
    QStringList layersStr;
    
    for (double layer : layers) {
        layersStr.append(QString::number(layer));
    }
    
    return layersStr.join(",");
}

void Simulation::setLayers(const QList<double> &layers) {
	this->layers = layers;
}

void Simulation::setLayersFromString(const QString &layersStr) {
    if (!layersStr.isEmpty()) {
        for (QString layer : layersStr.split(",")) {
            layers.append(layer.toDouble());
        }
    }
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

QStringList Simulation::getOutputParameters() const {
	return outputParameters;
}

void Simulation::setOutputParameters(const QStringList &outputParameters) {
	this->outputParameters = outputParameters;
}

SimulationStatus Simulation::getStatus() const {
    return status;
}

void Simulation::setStatus(const SimulationStatus &status) {
    this->previousStatus = this->status;
    this->status = status;
    
    if (this->simulationStruct) {
        this->simulationStruct->statusCode = (int) status;
    }
    
    if (status == SimulationStatus::FINISHED) {
        this->setProgress(100);
    }
    
    if (status != SimulationStatus::ABORTED) {
        emit updateStatus(status);
    }
}

SimulationStatus Simulation::getPreviousStatus() const {
    return previousStatus;
}

int Simulation::getOutputTimeInterval() const {
    return outputTimeInterval;
}

void Simulation::setOutputTimeInterval(int outputTimeInterval) {
    this->outputTimeInterval = outputTimeInterval;
}

int Simulation::getAutosaveTimeInterval() const {
    return autosaveTimeInterval;
}

void Simulation::setAutosaveTimeInterval(int autosaveTimeInterval) {
    this->autosaveTimeInterval = autosaveTimeInterval;
}

int Simulation::getProgress() const {
	return progress;
}

void Simulation::setProgress(int progress) {
	this->progress = progress;
	emit updateProgress(progress);
    
    if (this->progress == 100 && this->status != SimulationStatus::FINISHED) {
        setStatus(SimulationStatus::FINISHED);
    }
}

QString Simulation::getRecoveryVariables() const {
    return recoveryVariables;
}

void Simulation::setRecoveryVariables(const QString &recoveryVariables) {
    this->recoveryVariables = recoveryVariables;
}

void Simulation::buildRecoveryVariablesJson() {
    QString json("");
    
    if (simulationStruct && simulationStruct->recoveryVariables) {
        int uLength = simulationStruct->recoveryVariables->layers * simulationStruct->recoveryVariables->edges;
        QJsonObject jsonObject;
        QJsonArray u;
        
        jsonObject["layers"] = simulationStruct->recoveryVariables->layers;
        jsonObject["edges"] = simulationStruct->recoveryVariables->edges;
        jsonObject["elements"] = simulationStruct->recoveryVariables->elements;
        
        for (int i = 0; i < uLength; i++) {
            u.append(simulationStruct->recoveryVariables->u[i]);
        }
        
        jsonObject["u"] = u;
        
        int wLength = (simulationStruct->recoveryVariables->layers + 1) * simulationStruct->recoveryVariables->elements;
        QJsonArray w;
        
        for (int i = 0; i < wLength; i++) {
            w.append(simulationStruct->recoveryVariables->w[i]);
        }
        
        jsonObject["w"] = w;
        
        QJsonArray eta;
        
        for (int i = 0; i < simulationStruct->recoveryVariables->elements; i++) {
            eta.append(simulationStruct->recoveryVariables->eta[i]);
        }
        
        jsonObject["eta"] = eta;
        jsonObject["simulationTime"] = simulationStruct->recoveryVariables->simulationTime;
    
        json = QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);
    }
    
    this->recoveryVariables = json;
}

void Simulation::loadRecoveryVariables() {
    if (this->recoveryVariables.isEmpty()) {
        simulationStruct->recoveryVariables = nullptr;
    } else {
        QJsonObject jsonObject = QJsonDocument::fromJson(this->recoveryVariables.toUtf8()).object();
        
        simulationStruct->recoveryVariables = new SimulationDataType::RecoveryVariables();
        simulationStruct->recoveryVariables->layers = jsonObject["layers"].toInt();
        simulationStruct->recoveryVariables->edges = jsonObject["edges"].toInt();
        simulationStruct->recoveryVariables->elements = jsonObject["elements"].toInt();
        
        QJsonArray u = jsonObject["u"].toArray();
        
        simulationStruct->recoveryVariables->u = new double[u.size()];
        
        for (int i = 0; i < u.size(); i++) {
            simulationStruct->recoveryVariables->u[i] = u[i].toDouble();
        }
        
        QJsonArray w = jsonObject["w"].toArray();
        
        simulationStruct->recoveryVariables->w = new double[w.size()];
        
        for (int i = 0; i < w.size(); i++) {
            simulationStruct->recoveryVariables->w[i] = w[i].toDouble();
        }
        
        QJsonArray eta = jsonObject["eta"].toArray();
        
        simulationStruct->recoveryVariables->eta = new double[eta.size()];
        
        for (int i = 0; i < eta.size(); i++) {
            simulationStruct->recoveryVariables->eta[i] = eta[i].toDouble();
        }
        
        simulationStruct->recoveryVariables->simulationTime = jsonObject["simulationTime"].toDouble();
    }
}

QFileInfoList Simulation::getOutputFiles() const {
    QDir outputDir(this->outputDirectory);
    QFileInfoList outputFiles;
    
    if (outputDir.exists()) {
        QFileInfoList tempList = outputDir.entryInfoList({ QString("%1*.vtk").arg(label) }, QDir::Files, QDir::Time);
        
        for (int i = tempList.size() - 1; i >= 0; i--) {
            outputFiles.append(tempList.at(i));
        }
    }
    
    return outputFiles;
}

QMap<QString, LayerProperties*> Simulation::getSelectedLayers() const {
    return selectedLayers;
}

void Simulation::addSelectedLayer(const QString &layerKey) {
    QStringList layerAndComponent = layerKey.split("-");
    std::string layer = layerAndComponent.first().toStdString();
    std::string component = layerAndComponent.last().toStdString();
    
    selectedLayers.insert(layerKey, new LayerProperties());
}

Mesh* Simulation::getMesh() const {
    return hydrodynamicConfiguration->getGridDataConfiguration()->getMesh();
}

SimulationDataType::Simulation* Simulation::toSimulationDataType() {
	if (!simulationStruct) {
		Project *project = IPHApplication::getCurrentProject();
		std::string labelStr = this->label.toStdString();
		std::string outputDirectoryStr = this->outputDirectory.toStdString();
		int i = 0;
	    
	    simulationStruct = new SimulationDataType::Simulation();

	    simulationStruct->hydrodynamic = project->getHydrodynamic();
	    simulationStruct->waterQuality = project->getWaterQuality();
	    simulationStruct->sediment = project->getSediment();

	    simulationStruct->labelLength = this->label.size();
		simulationStruct->label = new char[simulationStruct->labelLength];
		strncpy(simulationStruct->label, labelStr.c_str(), simulationStruct->labelLength);
		simulationStruct->simulationType = (int) this->simulationType;
		simulationStruct->initialTime = this->initialTime;
		simulationStruct->period = this->period;
		simulationStruct->stepTime = this->stepTime;
		simulationStruct->layersLength = this->layers.size();
		simulationStruct->layers = new double[simulationStruct->layersLength];

		for (double depth : layers) {
			simulationStruct->layers[i++] = depth;
		}

		simulationStruct->hydrodynamicConfiguration = this->hydrodynamicConfiguration->toSimulationDataType();
	    simulationStruct->meteorologicalConfiguration = this->meteorologicalConfiguration->toSimulationDataType();
	    simulationStruct->minimumVerticalLimit = this->minimumVerticalLimit;
	    simulationStruct->maximumVerticalLimit = this->maximumVerticalLimit;
        
	    simulationStruct->outputDirectoryLength = this->outputDirectory.size();
		simulationStruct->outputDirectory = new char[simulationStruct->outputDirectoryLength];
		strncpy(simulationStruct->outputDirectory, outputDirectoryStr.c_str(), simulationStruct->outputDirectoryLength);

        simulationStruct->outputTimeInterval = this->outputTimeInterval;
        simulationStruct->autosaveTimeInterval = this->autosaveTimeInterval;
		simulationStruct->outputParametersLength = this->outputParameters.size();
	    simulationStruct->outputParameters = new SimulationDataType::OutputParameter[simulationStruct->outputParametersLength];
		i = 0;

		for (QString parameter : this->outputParameters) {
			std::string parameterStr = parameter.toStdString();
			simulationStruct->outputParameters[i].nameLength = parameter.size();
			simulationStruct->outputParameters[i].name = new char[parameter.size()];
			strncpy(simulationStruct->outputParameters[i].name, parameterStr.c_str(), parameter.size());
			i++;
		}
        
        simulationStruct->progress = this->progress;
        
        loadRecoveryVariables();
	}

	return simulationStruct;
}

QMap<SimulationType, QString> Simulation::getSimulationTypesMap() {
	return simulationTypesMap;
}

QMap<SimulationStatus, QString> Simulation::getSimulationStatusMap() {
	return simulationStatusMap;
}
