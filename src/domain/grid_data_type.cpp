#include "include/domain/grid_data_type.h"

GridDataType::GridDataType() : value(GridDataType::UNDEFINED) {}
GridDataType::GridDataType(const int &value) : value(value) {}

void GridDataType::setValue(const int &value) {
    this->value = value;
}

int GridDataType::getValue() const {
    return value;
}

GridDataType GridDataType::toGridDataType(const QString &gridDataTypeStr) {
    int value = UNDEFINED;

    if (gridDataTypeStr == "Bathymetry") {
        value = BATHYMETRY;
    }
    if (gridDataTypeStr == "Wind Reduction Coefficient") {
        value = WIND_REDUCTION;
    }
    if (gridDataTypeStr == "Chezy Coefficient") {
        value = CHEZY;
    }
    if (gridDataTypeStr == "Wetland Area") {
        value = WETLAND_AREA;
    }

    return GridDataType(value);
}

QString GridDataType::toString() {
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
