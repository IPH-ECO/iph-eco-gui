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
    vtkSmartPointer<vtkPolyData> polyData;
    
    // Transient attributes
    bool generationCanceled;
    bool showBoundaryEdges;
    bool showMesh;
    bool showUTMCoordinates;

private:
    void filterCoordinates(MeshPolygon *meshPolygon);

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
    vtkPolyData* getGrid();
    QString getMeshPolygonsAsString();
    QString getGridAsString();
    void loadMeshPolygonsFromStringPolyData(const QString &polyDataStr);
    
    MeshPolygon* addMeshPolygon(const QString &filename, const MeshPolygonType &meshPolygonType);
    void removeMeshPolygon(const QString &filename, const MeshPolygonType &meshPolygonType);
    MeshPolygon* getMeshPolygon(const QString &filename, const MeshPolygonType &meshPolygonType);

    void setShowBoundaryEdges(const bool &toogle);
    bool getShowBoundaryEdges() const;
    void setShowMesh(const bool &toogle);
    bool getShowMesh() const;

    double area();

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

