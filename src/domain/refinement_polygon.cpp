#include "include/domain/refinement_polygon.h"

#include <QtMath>

RefinementPolygon::RefinementPolygon() {}

RefinementPolygon::RefinementPolygon(const QString &filename, const double &maximumEdgeLength, const double &minimumAngle)
    : filename(filename), minimumAngle(minimumAngle), maximumEdgeLength(maximumEdgeLength) {}

QString RefinementPolygon::getFilename() const {
    return this->filename;
}

void RefinementPolygon::setFilename(const QString &filename) {
    this->filename = filename;
}

Polygon& RefinementPolygon::getPolygon() {
    return polygon;
}

void RefinementPolygon::setPolygon(const Polygon &polygon) {
    this->polygon = polygon;
}

void RefinementPolygon::setMinimumAngle(const double &minimumAngle) {
    // http://doc.cgal.org/latest/Mesh_2/classCGAL_1_1Delaunay__mesh__size__criteria__2.html
    this->minimumAngle = qPow(sin(minimumAngle * M_PI / 180), 2.0);
}

double RefinementPolygon::getMinimumAngle() const {
    return minimumAngle;
}

void RefinementPolygon::setMaximumEdgeLength(const double &maximumEdgeLength) {
    this->maximumEdgeLength = maximumEdgeLength;
}

double RefinementPolygon::getMaximumEdgeLength() const {
    return maximumEdgeLength;
}
