#ifndef GRID_DATA_CONFIGURATION_H
#define GRID_DATA_CONFIGURATION_H

#include <QVector>

#include "grid_data.h"

class GridDataConfiguration {
private:
    QString name;
    QVector<GridData*> gridDataVector;

public:
    GridDataConfiguration();

    QString getName() const;
    void setName(const QString &name);
    QVector<GridData*>& getGridDataVector();

    bool addGridData(GridData *gridData);
    void removeGridData(int i);
    bool containsGridData(GridData *gridData);
    GridData* getGridData(int i);
};

#endif // GRID_DATA_CONFIGURATION_H
