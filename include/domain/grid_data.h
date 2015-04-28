#ifndef GRID_DATA_H
#define GRID_DATA_H

#include <QObject>
#include <QSet>
#include "grid_information_type.h"
#include "grid_data_point.h"
#include "grid_data_polygon.h"

class GridData : public QObject {
    Q_OBJECT
public:
    enum GridInputType { POINT = 1, POLYGON };

private:
    GridInputType gridInputType;
    GridInformationType gridInformationType;
    QString inputFile;
    double exponent;
    double radius;
    bool show;
    QSet<GridDataPoint> dataPoints;
    GridDataPolygon dataPolygon;

public:
    GridData();

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
    QSet<GridDataPoint>& getGridDataPoints();
    GridDataPolygon& getGridDataPolygon();

    void buildDataPoints();
    void copy(GridData &srcGridData);

    QString gridInputTypeToString() const;

public slots:
    void setShow(bool show);
};

inline bool operator==(const GridDataPoint &dataPoint1, const GridDataPoint &dataPoint2) {
    return dataPoint1.getPoint().x() == dataPoint2.getPoint().x() && dataPoint1.getPoint().y() == dataPoint2.getPoint().y();
}

inline uint qHash(const GridDataPoint &dataPoint) {
    return qHash(QString::number(dataPoint.getPoint().x()) + QString::number(dataPoint.getPoint().y()));
}

#endif // GRID_DATA_H
