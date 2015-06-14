#ifndef GRID_DATA_CONFIGURATION_H
#define GRID_DATA_CONFIGURATION_H

#include "grid_data.h"

#include <QVector>

class GridDataConfiguration : public QObject {
    Q_OBJECT
private:
    QString name;
    QVector<GridData*> gridDataVector;
    
public:
    GridDataConfiguration();
    ~GridDataConfiguration();

    QString getName() const;
    void setName(const QString &name);
    QVector<GridData*> getGridDataVector();
    
    bool addGridData(GridData *gridData);
    void removeGridData(GridData *gridData);
    void removeGridData(int i);
    GridData* getGridData(const QString &gridDataName);
    GridData* getGridData(int i);
};

#endif // GRID_DATA_CONFIGURATION_H
