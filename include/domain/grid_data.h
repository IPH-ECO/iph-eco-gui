#ifndef GRID_DATA_H
#define GRID_DATA_H

#include <QObject>
#include <QSet>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

enum class GridDataInputType { POINT = 1, POLYGON };

enum class GridDataType { BATHYMETRY = 1, WIND_REDUCTION, CHEZY, WETLAND_AREA };

class GridData : public QObject {
    Q_OBJECT
private:
    uint id;
    QString name;
    GridDataInputType gridDataInputType;
    vtkSmartPointer<vtkPolyData> data;
    GridDataType gridDataType;
    QString inputFile;
    double exponent;
    double radius;

public:
    GridData();

    uint getId() const;
    void setId(const uint &id);
    QString getName() const;
    void setName(const QString &name);
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
    
    QString gridDataInputTypeToString() const;
    QString gridDataTypeToString() const;
    static GridDataType toGridDataType(const QString &gridDataTypeStr);

    void build();
};

#endif // GRID_DATA_H
