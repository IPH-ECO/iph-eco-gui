#ifndef MESH_POLYGON_H
#define MESH_POLYGON_H

#include <QString>
#include <vtkSmartPointer.h>
#include <vtkPolygon.h>

enum class MeshPolygonType { BOUNDARY, ISLAND, REFINEMENT_AREA };

class MeshPolygon {
private:
    uint id;
    MeshPolygonType meshPolygonType;
    vtkSmartPointer<vtkPolygon> originalPolygon;
    vtkSmartPointer<vtkPolygon> filteredPolygon;

    // Used by unstructured mesh
    double minimumAngle;
    double maximumEdgeLength;
    
    // Transient
    QString filename;

public:
    static const QString BOUNDARY_POLYGON_FILENAME;
    static const double DEFAULT_MINIMUM_ANGLE;
    static const double DEFAULT_MAXIMUM_EDGE_LENGTH;

    MeshPolygon();
    MeshPolygon(const QString &filename, const MeshPolygonType &meshPolygonType);

    void build();
    void filter(double &distanceFilter);
    bool pointInPolygon(double *point);
    double area();

    void setId(const uint &id);
    uint getId() const;
    void setFilename(const QString &filename);
    QString getFilename() const;
    void setMeshPolygonType(const MeshPolygonType &meshPolygonType);
    MeshPolygonType getMeshPolygonType() const;
    vtkPolygon* getOriginalPolygon() const;
    vtkPolygon* getFilteredPolygon() const;
    QString getPolyDataAsString();
    void loadPolygonsFromStringPolyData(const QString &polyDataStr);
    
    void setMinimumAngle(const double &minimumAngle);
    double getMinimumAngle() const;
    double getMinimumAngleInCGALRepresentation() const;
    void setMaximumEdgeLength(const double &maximumEdgeLength);
    double getMaximumEdgeLength() const;
    void setOptimalParameters();

    inline bool isBoundary() const {
        return this->meshPolygonType == MeshPolygonType::BOUNDARY;
    }

    inline bool isIsland() const {
        return this->meshPolygonType == MeshPolygonType::ISLAND;
    }

    inline bool isRefinementArea() const {
        return this->meshPolygonType == MeshPolygonType::REFINEMENT_AREA;
    }
};

#endif // MESH_POLYGON_H
