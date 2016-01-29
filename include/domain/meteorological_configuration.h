#ifndef METEOROLOGICAL_CONFIGURATION_H
#define METEOROLOGICAL_CONFIGURATION_H

#include "grid_data_configuration.h"
#include "meteorological_station.h"

class MeteorologicalConfiguration {
private:
    uint id;
    QString name; // configuration name
    GridDataConfiguration *gridDataConfiguration;
    QList<MeteorologicalStation*> stations;
public:
    MeteorologicalConfiguration();
    ~MeteorologicalConfiguration();
    uint getId() const;
    void setId(uint id);
    bool isPersisted() const;
    QString getName() const;
    void setName(const QString &name);
    GridDataConfiguration* getGridDataConfiguration() const;
    void setGridDataConfiguration(GridDataConfiguration *gridDataConfiguration);
    QList<MeteorologicalStation*> getStations() const;
    void setStations(const QList<MeteorologicalStation*> &stations);
    bool addStation(MeteorologicalStation *station);
    void removeStation(MeteorologicalStation *station);
    void removeStation(const QString &stationName);
    SimulationDataType::MeteorologicalConfiguration* toSimulationDataType() const;
};

#endif // METEOROLOGICAL_CONFIGURATION_H
