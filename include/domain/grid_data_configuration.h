#ifndef GRID_DATA_CONFIGURATION_H
#define GRID_DATA_CONFIGURATION_H

#include "simulation_data_type.h"
#include "grid_data.h"

#include <QVector>

class GridData;
enum class GridDataType;

class GridDataConfiguration : public QObject {
    Q_OBJECT
private:
    uint id;
    QString name;
    QVector<GridData*> gridDataVector; // TODO: change to qset
    
public:
    GridDataConfiguration();
    ~GridDataConfiguration();

    uint getId() const;
    void setId(const uint &id);
    bool isPersisted() const;
    QString getName() const;
    void setName(const QString &name);
    QVector<GridData*> getGridDataVector();
    
    bool addGridData(GridData *gridData);
    void removeGridData(GridData *gridData);
    void removeGridData(int i);
    GridData* getGridData(const QString &gridDataName);
    GridData* getGridData(int i);
    QList<GridData*> getGridData(const GridDataType &gridDataType) const;
    void clearGridDataVector();
    Mesh* getMesh() const;
    
    SimulationDataType::GridDataConfiguration toSimulationDataType() const;
};

#endif // GRID_DATA_CONFIGURATION_H
