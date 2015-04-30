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

    static GridInformationType toGridInformationType(const QString &gridInformationTypeStr);

    GridInformationType();
    GridInformationType(const int &value);
    void setValue(const int &value);
    int getValue() const;
    QString toString();

    inline bool operator==(const GridInformationType &gridInformationType) {
        return this->value == gridInformationType.getValue();
    }
};

#endif // GRID_INFORMATION_TYPE_H

