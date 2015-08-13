#ifndef MESH_H
#define MESH_H

#include "mesh_polygon.h"
#include <QHash>
#include <QList>
#include <QObject>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

class Mesh : public QObject {
    Q_OBJECT
protected:
    uint id;
    QString name;
    MeshPolygon *boundaryPolygon;
    double coordinatesDistance;
    QList<MeshPolygon*> islands;
    QList<MeshPolygon*> refinementAreas;
    vtkSmartPointer<vtkPolyData> meshPolyData;
    vtkSmartPointer<vtkPolyData> boundaryPolyData;

    // Transient attributes
    bool generationCanceled;
    
    void generateBoundaryPolyData();

public:
    Mesh();
    virtual ~Mesh();

    void setId(const uint &id);
    uint getId() const;
    void setName(const QString &name);
    QString getName() const;
    void setCoordinatesDistance(const double &coordinatesDistance);
    double getCoordinatesDistance() const;
    MeshPolygon* getBoundaryPolygon();
    QList<MeshPolygon*> getIslands();
    QList<MeshPolygon*> getRefinementAreas();
    vtkSmartPointer<vtkPolyData> getMeshPolyData() const;
    vtkSmartPointer<vtkPolyData> getBoundaryPolyData() const;
    QString getMeshPolygonsAsString();
    QString getMeshPolyDataAsString() const;
    QString getBoundaryPolyDataAsString() const;
    void loadMeshPolyDataFromString(const QString &polyDataStr);
    void loadBoundaryPolyDataFromString(const QString &polyDataStr);
    
    MeshPolygon* addMeshPolygon(const QString &name, const QString &filename, const MeshPolygonType &meshPolygonType);
    void addMeshPolygon(MeshPolygon *meshPolygon);
    void removeMeshPolygon(const QString &name, const MeshPolygonType &meshPolygonType);
    MeshPolygon* getMeshPolygon(const QString &name, const MeshPolygonType &meshPolygonType) const;

    double area();
    bool hasArray(const QString &arrayName);
    void removeArray(const QString &arrayName);

    virtual bool instanceOf(const QString &type) = 0;
    virtual void generate() = 0;
    virtual void clear();
    
    bool isPersisted() const;
signals:
    void updateProgress(int value);
    
public slots:
    void cancelGeneration(bool value = true);
};

#endif // MESH_H