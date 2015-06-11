#ifndef GRID_DATA_H
#define GRID_DATA_H

#include "grid_data_type.h"

#include <QObject>
#include <QSet>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

enum class GridDataInputType { POINT = 1, POLYGON };

class GridData : public QObject {
    Q_OBJECT
private:
    GridDataInputType gridDataInputType;
    vtkSmartPointer<vtkPolyData> data;
    GridDataType gridDataType;
    QString inputFile;
    double exponent;
    double radius;
    bool show; //?

public:
    GridData();

    GridDataInputType getGridDataInputType() const;
    void setGridDataInputType(const GridDataInputType &gridDataInputType);
    GridDataType getGridDataType() const;
    void setGridDataType(const GridDataType &gridDataType);
    QString getInputFile() const;
    void setInputFile(const QString &inputFile);
    double getExponent() const;
    void setExponent(const double &exponent);
    double getRadius() const;
    void setRadius(const double &radius);
    vtkPolyData* getData() const;
    bool getShow() const; //?

    void build();

    QString gridDataInputTypeToString() const;

public slots:
    void setShow(bool show);
};

#endif // GRID_DATA_H
