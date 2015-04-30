#ifndef GRID_INFORMATION_TYPE_H
#define GRID_INFORMATION_TYPE_H

#include <QString>

class GridInformationType {
private:
    int value;

public:
    static const int BATHYMETRY = 1;
    static const int WIND_REDUCTION = 2;
    static const int CHEZY = 3;
    static const int WETLAND_AREA = 4;
    static const int UNDEFINED = -1;

    GridInformationType() : value(GridInformationType::UNDEFINED) {}
    GridInformationType(const int &value) : value(value) {}

    void setValue(const int &value) {
        this->value = value;
    }

    int getValue() const {
        return value;
    }

    inline bool operator==(const GridInformationType &gridInformationType) {
        return this->value == gridInformationType.getValue();
    }

    static GridInformationType toGridInformationType(const QString &gridInformationTypeStr) {
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

    QString toString() {
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
};

#endif // GRID_INFORMATION_TYPE_H

