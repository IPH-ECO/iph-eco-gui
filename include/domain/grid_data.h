#ifndef GRID_DATA_H
#define GRID_DATA_H

#include "mesh.h"
#include "grid_data_configuration.h"

#include <vtkSmartPointer.h>
#include <vtkIdTypeArray.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <QObject>
#include <QSet>

class GridDataConfiguration;

enum class GridDataInputType { POINT = 1, POLYGON };

enum class GridDataType { BATHYMETRY = 1, WIND_REDUCTION, CHEZY, WETLAND_AREA };

class GridData : public QObject {
    Q_OBJECT
private:
    uint id;
    QString name;
    GridDataType gridDataType;
    GridDataInputType gridDataInputType;
    vtkSmartPointer<vtkPolyData> inputPolyData;
    double exponent;
    double radius;
    GridDataConfiguration *gridDataConfiguration;
    Mesh *mesh;
    
    // Transient attributes
    QString inputFile;
    bool interpolationCanceled;
    
    double inverseOfDistance(vtkIdTypeArray *inscribedPointsIndexes, double *cellCenter);
    double calculateNearestWeight(double *cellCenter);

public:
    GridData(Mesh *mesh);

    uint getId() const;
    void setId(const uint &id);
    QString getName() const;
    void setName(const QString &name);
    GridDataType getGridDataType() const;
    void setGridDataType(const GridDataType &gridDataType);
    GridDataInputType getGridDataInputType() const;
    void setGridDataInputType(const GridDataInputType &gridDataInputType);
    vtkPolyData* getInputPolyData();
    vtkPolyData* getInterpolatedPolyData();
    double getExponent() const;
    void setExponent(const double &exponent);
    double getRadius() const;
    void setRadius(const double &radius);
    GridDataConfiguration* getGridDataConfiguration() const;
    void setGridDataConfiguration(GridDataConfiguration *gridDataConfiguration);
    Mesh* getMesh() const;
    void setMesh(Mesh *mesh);
    
    QString getInputFile() const;
    void setInputFile(const QString &inputFile);
    QString gridDataInputTypeToString() const;
    QString gridDataTypeToString() const;
    static GridDataType toGridDataType(const QString &gridDataTypeStr);
    
    void loadInputPolyDataFromStringPolyData(const QString &polyDataStr);
    QString getInputPolyDataAsString() const;

    void buildInputPolyData();
    void interpolate();
    bool isPersisted() const;
    
signals:
    void updateProgress(int value);
    
public slots:
    void cancelInterpolation(bool value = true);
};

#endif // GRID_DATA_H
