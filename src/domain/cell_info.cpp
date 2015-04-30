#include "include/domain/cell_info.h"

CellInfo::CellInfo() {}
CellInfo::CellInfo(const GridInformationType &gridInformationType, double &weight) : gridInformationType(gridInformationType), weight(weight) {}

void CellInfo::setGridInformationType(GridInformationType &gridInformationType) {
    this->gridInformationType = gridInformationType;
}

GridInformationType CellInfo::getGridInformationType() const {
    return gridInformationType;
}

void CellInfo::setWeight(const double &weight) {
    this->weight = weight;
}

double CellInfo::getWeight() const {
    return weight;
}
