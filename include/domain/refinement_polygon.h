#ifndef REFINEMENT_POLYGON_H
#define REFINEMENT_POLYGON_H

#include <QString>

#include "include/utility/delaunay_triangulation_definitions.h"

class RefinementPolygon {
private:
    QString filename;
    Polygon polygon;
    double minimumAngle;
    double maximumEdgeLength;

public:
    RefinementPolygon();
    RefinementPolygon(const QString &filename, const double &maximumEdgeLength, const double &minimumAngle);

    QString getFilename() const;
    void setFilename(const QString &filename);

    Polygon& getPolygon();
    void setPolygon(const Polygon &polygon);

    void setMinimumAngle(const double &minimumAngle);
    double getMinimumAngle() const;

    void setMaximumEdgeLength(const double &maximumEdgeLength);
    double getMaximumEdgeLength() const;
};

#endif // REFINEMENT_POLYGON_H
