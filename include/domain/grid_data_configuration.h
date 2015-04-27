#ifndef GRID_DATA_CONFIGURATION_H
#define GRID_DATA_CONFIGURATION_H

#include <QVector>

#include "grid_data.h"
#include "mesh.h"

class GridDataConfiguration {
private:
    QString name;
    QVector<GridData*> gridDataVector;
    Mesh *mesh;

    double inverseOfDistance(GridData *gridData, QSet<GridDataPoint> &dataPoints, Point &centroid);

public:
    GridDataConfiguration();
    ~GridDataConfiguration();

    QString getName() const;
    void setName(const QString &name);
    Mesh* getMesh() const;
    void setMesh(Mesh *mesh);

    QVector<GridData*>& getGridDataVector();

    bool addGridData(GridData *gridData);
    void removeGridData(int i);
    bool containsGridData(GridData *gridData);
    GridData* getGridData(int i);
    void processGridData(GridData *gridData);
};

#endif // GRID_DATA_CONFIGURATION_H
