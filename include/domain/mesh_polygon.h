#ifndef MESH_POLYGON_H
#define MESH_POLYGON_H

#include <QString>
#include <vtkSmartPointer.h>
#include <vtkPolygon.h>

enum class MeshPolygonType { BOUNDARY = 1, ISLAND, REFINEMENT_AREA };
enum class CoordinateSystem { LATITUDE_LONGITUDE, UTM };

class MeshPolygon {
private:
    uint id;
    QString name;
    MeshPolygonType meshPolygonType;
    vtkSmartPointer<vtkPolygon> originalPolygon;
    vtkSmartPointer<vtkPolygon> filteredPolygon;
    double latitudeAverage;

    // Used by unstructured mesh
    double minimumAngle;
    double maximumEdgeLength;
    
    // Transient
    QString filename;
    
    void readFromKMLFile(const CoordinateSystem &coordinateSystem);
    void readFromTextFile(const CoordinateSystem &coordinateSystem);

public:
    static const QString BOUNDARY_POLYGON_NAME;
    static const double DEFAULT_MINIMUM_ANGLE;
    static const double DEFAULT_MAXIMUM_EDGE_LENGTH;

    MeshPolygon();
    MeshPolygon(const QString &name, const QString &filename, const MeshPolygonType &meshPolygonType);

    void setId(const uint &id);
    uint getId() const;
    void setName(const QString &name);
    QString getName() const;
    void setFilename(const QString &filename);
    QString getFilename() const;
    void setMeshPolygonType(const MeshPolygonType &meshPolygonType);
    MeshPolygonType getMeshPolygonType() const;
    vtkPolygon* getOriginalPolygon() const;
    vtkPolygon* getFilteredPolygon() const;
    QString getOriginalPolyDataAsString();
	QString getFilteredPolyDataAsString();
	void loadOriginalPolygonFromStringPolyData(const QString &polyDataStr);
	void loadFilteredPolygonFromStringPolyData(const QString &polyDataStr);
    double getLatitudeAverage() const;
    void setLatitudeAverage(double latitudeAverage);
    
    void setMinimumAngle(const double &minimumAngle);
    double getMinimumAngle() const;
    double getMinimumAngleInCGALRepresentation() const;
    void setMaximumEdgeLength(const double &maximumEdgeLength);
    double getMaximumEdgeLength() const;
    void setInitialCriteria();
    
    void build(const CoordinateSystem &coordinateSystem);
    void filter(const double &distanceFilter);
    bool pointInPolygon(double *point);
    bool isPersisted() const;

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
