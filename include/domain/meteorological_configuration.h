#ifndef METEOROLOGICAL_CONFIGURATION_H
#define METEOROLOGICAL_CONFIGURATION_H

#include "mesh.h"
#include "meteorological_station.h"

class MeteorologicalConfiguration {
private:
    uint id;
    QString name; // configuration name
    Mesh *mesh;
    QList<MeteorologicalStation*> stations;
public:
    MeteorologicalConfiguration();
    uint getId() const;
    void setId(uint id);
    bool isPersisted() const;
    QString getName() const;
    void setName(const QString &name);
    Mesh* getMesh() const;
    void setMesh(Mesh *mesh);
    QList<MeteorologicalStation*> getStations() const;
    void setStations(const QList<MeteorologicalStation*> &stations);
    bool addStation(MeteorologicalStation *station);
    void removeStation(MeteorologicalStation *station);
    void removeStation(const QString &stationName);
};

#endif // METEOROLOGICAL_CONFIGURATION_H
