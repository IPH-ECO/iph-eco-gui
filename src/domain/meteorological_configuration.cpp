#include "include/domain/meteorological_configuration.h"

MeteorologicalConfiguration::MeteorologicalConfiguration() : id(0) {}

uint MeteorologicalConfiguration::getId() const {
    return id;
}

void MeteorologicalConfiguration::setId(uint id) {
    if (!isPersisted()) {
        this->id = id;
    }
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

Mesh* MeteorologicalConfiguration::getMesh() const {
    return mesh;
}

void MeteorologicalConfiguration::setMesh(Mesh *mesh) {
    this->mesh = mesh;
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