#ifndef REFINEMENT_AREA_H
#define REFINEMENT_AREA_H

#include <QString>

#include "mesh_polygon.h"

class RefinementArea {
private:
    QString filename;
    MeshPolygon meshPolygon;
    double minimumAngle;
    double maximumEdgeLength;

public:
    RefinementArea();
    RefinementArea(const QString &filename);

    QString getFilename() const;
    void setFilename(const QString &filename);

    MeshPolygon& getMeshPolygon();
    void setMeshPolygon(const MeshPolygon &meshPolygon);

    void setMinimumAngle(const double &minimumAngle);
    double getMinimumAngle() const;

    void setMaximumEdgeLength(const double &maximumEdgeLength);
    double getMaximumEdgeLength() const;
};

#endif // REFINEMENT_AREA_H
