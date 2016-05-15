#ifndef GRID_DATA_H
#define GRID_DATA_H

#include "mesh.h"
#include "grid_data_configuration.h"
#include "layer_properties.h"

#include <vtkSmartPointer.h>
#include <vtkIdTypeArray.h>
#include <vtkPolyData.h>
#include <QSet>

class GridDataConfiguration;

enum class GridDataInputType { POINT = 1, POLYGON };
enum class GridDataType { BATHYMETRY = 1, ROUGHNESS, WIND_REDUCTION, WETLAND_AREA, D50_GRAIN_SIZE, FRACTION_OF_ORGANIC_MATTER, IMPERVIOUS_BEDROCK_LEVEL };

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
    LayerProperties *layerProperties;
    Mesh *mesh;
    
    static QMap<GridDataType, QString> gridTypesMap;
    
    // Transient attributes
    QString inputFile;
    bool interpolationCanceled;
    CoordinateSystem coordinateSystem;
    
    double inverseOfDistance(vtkSmartPointer<vtkIdList> inscribedPointsIds, double *cellCenter);

public:
    GridData(Mesh *mesh);
    ~GridData();

    uint getId() const;
    void setId(const uint &id, const bool &force = false);
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
    LayerProperties* getLayerProperties() const;
    void setLayerProperties(LayerProperties *layerProperties);
    Mesh* getMesh() const;
    void setMesh(Mesh *mesh);
    CoordinateSystem getCoordinateSystem() const;
    void setCoordinateSystem(const CoordinateSystem &coordinateSystem);
    
    QString getInputFile() const;
    void setInputFile(const QString &inputFile);
    QString gridDataInputTypeToString() const;
    QString gridDataTypeToString() const;
    
    static QMap<GridDataType, QString> getGridTypesMap();
    
    void loadInputPolyDataFromStringPolyData(const QString &polyDataStr);
    QString getInputPolyDataAsString() const;

    void buildInputPolyData();
    void interpolate();
    bool isPersisted() const;
	int getMaximumProgress() const;
    double getMinimumWeight() const;
    
    SimulationDataType::GridData toSimulationDataType() const;

signals:
	void updateProgressText(const QString &text);
    void updateProgress(int value);
    
public slots:
    void cancelInterpolation(bool value = true);
};

#endif // GRID_DATA_H
