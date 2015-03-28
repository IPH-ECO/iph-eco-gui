#include "include/domain/refinement_area.h"

#include <QtMath>

RefinementArea::RefinementArea() {}

RefinementArea::RefinementArea(const QString &filename) : filename(filename) {}

QString RefinementArea::getFilename() const {
    return this->filename;
}

void RefinementArea::setFilename(const QString &filename) {
    this->filename = filename;
}

MeshPolygon* RefinementArea::getMeshPolygon() {
    return const_cast<MeshPolygon*>(&meshPolygon);
}

void RefinementArea::setMeshPolygon(const MeshPolygon &meshPolygon) {
    this->meshPolygon = meshPolygon;
}

void RefinementArea::setMinimumAngle(const double &minimumAngle) {
    // http://doc.cgal.org/latest/Mesh_2/classCGAL_1_1Delaunay__mesh__size__criteria__2.html
    this->minimumAngle = qPow(sin(minimumAngle * M_PI / 180), 2.0);
}

double RefinementArea::getMinimumAngle() const {
    return minimumAngle;
}

double RefinementArea::getMinimumAngleInDegrees() const {
    return qAsin(qSqrt(minimumAngle)) * 180 / M_PI;
}

void RefinementArea::setMaximumEdgeLength(const double &maximumEdgeLength) {
    this->maximumEdgeLength = maximumEdgeLength;
}

double RefinementArea::getMaximumEdgeLength() const {
    return maximumEdgeLength;
}

double RefinementArea::getUpperBoundForMaximumEdgeLength() const {
    return qMin(meshPolygon.width(), meshPolygon.height());
}
