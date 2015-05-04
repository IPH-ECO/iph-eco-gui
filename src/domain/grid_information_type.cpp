#include "include/domain/grid_information_type.h"

GridInformationType::GridInformationType() : value(GridInformationType::UNDEFINED) {}
GridInformationType::GridInformationType(const int &value) : value(value) {}

void GridInformationType::setValue(const int &value) {
    this->value = value;
}

int GridInformationType::getValue() const {
    return value;
}

GridInformationType GridInformationType::toGridInformationType(const QString &gridInformationTypeStr) {
    int value = UNDEFINED;

    if (gridInformationTypeStr == "Bathymetry") {
        value = BATHYMETRY;
    }
    if (gridInformationTypeStr == "Wind Reduction Coefficient") {
        value = WIND_REDUCTION;
    }
    if (gridInformationTypeStr == "Chezy Coefficient") {
        value = CHEZY;
    }
    if (gridInformationTypeStr == "Wetland Area") {
        value = WETLAND_AREA;
    }

    return GridInformationType(value);
}

QString GridInformationType::toString() {
    switch (value) {
    case BATHYMETRY:
        return "Bathymetry";
    case WIND_REDUCTION:
        return "Wind Reduction Coefficient";
    case CHEZY:
        return "Chezy Coefficient";
    case WETLAND_AREA:
        return "Wetland Area";
    default:
        return "";
    }
}
