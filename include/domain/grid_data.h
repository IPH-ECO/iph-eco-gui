#ifndef GRID_DATA_H
#define GRID_DATA_H

#include <QObject>
#include <QSet>
#include "grid_data_point.h"

class GridData : public QObject {
    Q_OBJECT
public:
    enum GridInputType { POINT = 1, POLYGON };
    enum GridInformationType { BATHYMETRY = 1, WIND_REDUCTION, CHEZY, WETLAND_AREA, UNDEFINED };

private:
    GridInputType gridInputType;
    GridInformationType gridInformationType;
    QString inputFile;
    double exponent;
    double radius;
    bool show;
    QSet<GridDataPoint> dataPoints;

public:
    GridData();

    QString getName() const;
    void setName(const QString &name);
    GridInputType getGridInputType() const;
    void setGridInputType(const GridInputType gridInputType);
    GridInformationType getGridInformationType() const;
    void setGridInformationType(const GridInformationType &gridInformationType);
    QString getInputFile() const;
    void setInputFile(const QString &inputFile);
    double getExponent() const;
    void setExponent(const double &exponent);
    double getRadius() const;
    void setRadius(const double &radius);
    bool getShow() const;
    QSet<GridDataPoint>& getDataPoints();

    void buildDataPoints();
    void copy(GridData &srcGridData);

    QString gridInputTypeToString() const;
    QString gridInformationTypeToString() const;

    static GridInformationType toGridInformationType(const QString gridInformationTypeStr);

public slots:
    void setShow(bool show);
};

inline bool operator==(const GridDataPoint &dataPoint1, const GridDataPoint &dataPoint2) {
    return dataPoint1.getX() == dataPoint2.getX() && dataPoint1.getY() == dataPoint2.getY();
}

inline uint qHash(const GridDataPoint &dataPoint) {
    return qHash(QString::number(dataPoint.getX()) + QString::number(dataPoint.getY()));
}

#endif // GRID_DATA_H
