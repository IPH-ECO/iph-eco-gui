#ifndef CELL_H
#define CELL_H

#include "grid_information_type.h"

class Cell {
private:
    GridInformationType gridInformationType;
    double weight;

public:
    Cell() {}
    Cell(GridInformationType &gridInformationType, double &weight) : gridInformationType(gridInformationType), weight(weight) {}

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

    bool isValid() {
        return gridInformationType.getValue() != GridInformationType::UNDEFINED;
    }
};

#endif // CELL_H

