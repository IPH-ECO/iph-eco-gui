#ifndef GRID_DATA_H
#define GRID_DATA_H

#include "mesh.h"

#include <vtkSmartPointer.h>
#include <vtkIdTypeArray.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <QObject>
#include <QSet>

enum class GridDataInputType { POINT = 1, POLYGON };

enum class GridDataType { BATHYMETRY = 1, WIND_REDUCTION, CHEZY, WETLAND_AREA };

class GridData : public QObject {
    Q_OBJECT
private:
    uint id;
    QString name;
    GridDataInputType gridDataInputType;
    vtkSmartPointer<vtkPolyData> inputPolyData;
    vtkSmartPointer<vtkPolyData> interpolatedPolyData;
    GridDataType gridDataType;
    QString inputFile;
    double exponent;
    double radius;
    Mesh *mesh;
    
    // Transient attributes
    bool interpolationCanceled;
    
    double inverseOfDistance(vtkIdTypeArray *inscribedPointsIndexes, double *cellCenter);
    double calculateNearestWeight(double *cellCenter);

public:
    GridData(Mesh *mesh);

    uint getId() const;
    void setId(const uint &id);
    QString getName() const;
    void setName(const QString &name);
    GridDataInputType getGridDataInputType() const;
    void setGridDataInputType(const GridDataInputType &gridDataInputType);
    vtkPolyData* getInputPolyData();
    vtkPolyData* getInterpolatedPolyData();
    GridDataType getGridDataType() const;
    void setGridDataType(const GridDataType &gridDataType);
    QString getInputFile() const;
    void setInputFile(const QString &inputFile);
    double getExponent() const;
    void setExponent(const double &exponent);
    double getRadius() const;
    void setRadius(const double &radius);
    Mesh* getMesh() const;
    void setMesh(Mesh *mesh);
    
    QString gridDataInputTypeToString() const;
    QString gridDataTypeToString() const;
    static GridDataType toGridDataType(const QString &gridDataTypeStr);

    void buildInputPolyData();
    void interpolate();
    
signals:
    void updateProgress(int value);
    
public slots:
    void cancelInterpolation(bool value = true);
};

#endif // GRID_DATA_H
