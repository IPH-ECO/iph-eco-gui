#ifndef GRID_DATA_TYPE_H
#define GRID_DATA_TYPE_H

#include <QString>

class GridDataType {
private:
    int value;

public:
    static const int BATHYMETRY = 1;
    static const int WIND_REDUCTION = 2;
    static const int CHEZY = 3;
    static const int WETLAND_AREA = 4;
    static const int UNDEFINED = -1;

    static GridDataType toGridDataType(const QString &gridDataTypeStr);

    GridDataType();
    GridDataType(const int &value);
    void setValue(const int &value);
    int getValue() const;
    QString toString();

    inline bool operator==(const GridDataType &gridDataType) {
        return this->value == gridDataType.getValue();
    }
};

#endif // GRID_DATA_TYPE_H

