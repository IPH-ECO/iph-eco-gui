#ifndef CELL_INFO_H
#define CELL_INFO_H

#include "grid_information_type.h"

class CellInfo {
private:
    GridInformationType gridInformationType;
    double weight;

public:
    CellInfo();
    CellInfo(const GridInformationType &gridInformationType, double &weight);

    void setGridInformationType(GridInformationType &gridInformationType);
    GridInformationType getGridInformationType() const;
    void setWeight(const double &weight);
    double getWeight() const;
};

#endif // CELL_INFO_H

