#ifndef GRID_DATA_H
#define GRID_DATA_H

#include <QString>

#include "mesh.h"

class GridData {
private:
    QString name;
    QString bathymetryData;
    QString chezyCoefficientData;
    QString windReductionCoefficientData;
    QString wetlandAreasData;
    Mesh *mesh;

public:
    GridData();
    GridData(Mesh *mesh);

    QString getName() const;
    void setName(const QString &name);
    Mesh* getMesh() const;
    void setMesh(Mesh *mesh);
};

#endif // GRID_DATA_H
