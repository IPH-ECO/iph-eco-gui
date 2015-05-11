#ifndef CELL_INFO_H
#define CELL_INFO_H

#include "grid_information_type.h"

class GridData;

class CellInfo {
private:
    GridData *gridData;
    GridInformationType gridInformationType;
    double weight;

public:
    CellInfo();
    CellInfo(GridData* gridData, const GridInformationType &gridInformationType, double &weight);

    void setGridInformationType(GridInformationType &gridInformationType);
    GridInformationType getGridInformationType() const;
    void setWeight(const double &weight);
    double getWeight() const;
    void setGridData(GridData *gridData);
    GridData* getGridData() const;
};

#endif // CELL_INFO_H

