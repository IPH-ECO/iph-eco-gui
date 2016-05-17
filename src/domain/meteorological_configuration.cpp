#include <domain/meteorological_configuration.h>

MeteorologicalConfiguration::MeteorologicalConfiguration() : id(0) {}

MeteorologicalConfiguration::~MeteorologicalConfiguration() {
    for (MeteorologicalStation *station : stations) {
        delete station;
    }
}

uint MeteorologicalConfiguration::getId() const {
    return id;
}

void MeteorologicalConfiguration::setId(const uint &id) {
    this->id = id;
}

bool MeteorologicalConfiguration::isPersisted() const {
    return id != 0;
}

QString MeteorologicalConfiguration::getName() const {
    return name;
}

void MeteorologicalConfiguration::setName(const QString &name) {
    this->name = name;
}

GridDataConfiguration* MeteorologicalConfiguration::getGridDataConfiguration() const {
    return gridDataConfiguration;
}

void MeteorologicalConfiguration::setGridDataConfiguration(GridDataConfiguration *gridDataConfiguration) {
    this->gridDataConfiguration = gridDataConfiguration;
}

QList<MeteorologicalStation*> MeteorologicalConfiguration::getStations() const {
    return stations;
}

void MeteorologicalConfiguration::setStations(const QList<MeteorologicalStation*> &stations) {
    this->stations = stations;
}

bool MeteorologicalConfiguration::addStation(MeteorologicalStation *station) {
    if (stations.contains(station)) {
        return false;
    }
    
    stations.append(station);
    
    return true;
}

void MeteorologicalConfiguration::removeStation(MeteorologicalStation *station) {
    stations.removeOne(station);
}

void MeteorologicalConfiguration::removeStation(const QString &stationName) {
    for (MeteorologicalStation *station : stations) {
        if (station->getName() == stationName) {
            stations.removeOne(station);
        }
    }
}

SimulationDataType::MeteorologicalConfiguration* MeteorologicalConfiguration::toSimulationDataType() const {
    SimulationDataType::MeteorologicalConfiguration *configuration = new SimulationDataType::MeteorologicalConfiguration();
    
    configuration->stationsLength = this->stations.size();
    configuration->stations = new SimulationDataType::MeteorologicalStation[this->stations.size()];
    
    for (int i = 0; i < this->stations.size(); i++) {
        configuration->stations[i] = this->stations[i]->toSimulationDataType();
    }
    
    return configuration;
}