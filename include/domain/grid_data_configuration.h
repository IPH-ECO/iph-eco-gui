#ifndef GRID_DATA_CONFIGURATION_H
#define GRID_DATA_CONFIGURATION_H

#include "grid_data.h"
#include "mesh.h"

#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <QVector>

class GridDataConfiguration : public QObject {
    Q_OBJECT
private:
    QString name;
    Mesh *mesh;
    QVector<GridData*> gridDataVector;
    bool interpolationCanceled;
    bool showMesh;

    double inverseOfDistance(vtkPoints *inscribedPoints, double *cellCenter, const double &exponent);
    double calculateNearestWeight(vtkPolyData *gridPolyData, double *cellCenter);

public:
    GridDataConfiguration();
    ~GridDataConfiguration();

    QString getName() const;
    void setName(const QString &name);
    Mesh* getMesh() const;
    void setMesh(Mesh *mesh);
    QVector<GridData*> getGridDataVector();
    void interpolate(GridData *gridData);
    bool interpolationWasCanceled() const;
    bool getShowMesh() const;
    void setShowMesh(const bool showMesh);

    bool addGridData(GridData *gridData);
    void removeGridData(GridData *gridData);
    void removeGridData(int i);
    bool containsGridData(GridData *gridData);
    GridData* getGridData(int i);
    
signals:
    void updateProgress(int value);

public slots:
    void cancelInterpolation(bool value = true);
};

#endif // GRID_DATA_CONFIGURATION_H
