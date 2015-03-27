#ifndef UNSTRUCTUREDMESH_H
#define UNSTRUCTUREDMESH_H

#include "include/utility/delaunay_triangulation_definitions.h"
#include "include/exceptions/mesh_exception.h"
#include "mesh.h"

#include <QList>

class UnstructuredMesh : public Mesh {
private:
    const double DEFAULT_MINIMUM_ANGLE;
    const double DEFAULT_MINIMUM_EDGE_LENGTH;

    CDT cdt;
    double minimumAngle;
    double maximumEdgeLength;
    QVector<RefinementArea> refinementAreas;

    void calculateOptimalEdgeLength(const MeshPolygon *polygon);
    void mark_domains(CDT::Face_handle start, int index, QList<CDT::Edge>& border);
    void mark_domains();

public:
    UnstructuredMesh();
    UnstructuredMesh(QString &_name);
    UnstructuredMesh(QString &_name, QString &_boundaryFilePath);
    UnstructuredMesh(QString &_name, QString &_boundaryFilePath, double &_minimumAngle, double &_maximumEdgeLength);

    void setMinimumAngle(const double &minimumAngle);
    double getMinimumAngle() const;
    void setMaximumEdgeLength(const double &maximumEdgeLength);
    double getMaximumEdgeLength() const;
    double getUpperBoundForMaximumEdgeLength() const;

    void generate();

    const CDT* getCDT();

    QVector<RefinementArea>& getRefinementPolygons();
    void addRefinementPolygon(QString &filename);
    void removeRefinementPolygon(const QString &filename);
    const RefinementArea* getRefinementPolygon(const QString &filename);

    virtual void buildDomain();
    virtual void clear();
};

#endif // UNSTRUCTUREDMESH_H
