#ifndef CELL_INFO_H
#define CELL_INFO_H

#include "grid_information_type.h"

class CellInfo {
private:
    GridInformationType gridInformationType;
    double weight;

public:
    CellInfo() {}
    CellInfo(const GridInformationType &gridInformationType, double &weight) : gridInformationType(gridInformationType), weight(weight) {}

    void setGridInformationType(GridInformationType &gridInformationType) {
        this->gridInformationType = gridInformationType;
    }

    GridInformationType getGridInformationType() const {
        return gridInformationType;
    }

    void setWeight(const double &weight) {
        this->weight = weight;
    }

    double getWeight() const {
        return weight;
    }
};

#endif // CELL_INFO_H

