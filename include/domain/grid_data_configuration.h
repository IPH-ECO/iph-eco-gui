#ifndef GRID_DATA_CONFIGURATION_H
#define GRID_DATA_CONFIGURATION_H

#include <QSet>

#include "grid_data.h"

class GridDataConfiguration {
private:
    QString &name;
    QSet<GridData*> gridDataSet;

public:
    GridDataConfiguration();

    QString getName() const;
    void setName(const QString &name);
    QSet<GridData*>& getGridDataSet() const;

    bool addGridData(GridData *gridData);
    void removeGridData(GridData *gridData);
    bool containsGridData(GridData *gridData);
    GridData* getGridData(const GridData::GridInputType &gridInputType, const QString &gridInputFile, const GridData::GridInformationType gridInformationType);
};

#endif // GRID_DATA_CONFIGURATION_H
