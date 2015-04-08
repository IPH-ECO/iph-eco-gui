#include "include/domain/mesh_polygon.h"

#include <QtMath>

const double MeshPolygon::DEFAULT_MINIMUM_ANGLE = 20.7;

const double MeshPolygon::DEFAULT_MAXIMUM_EDGE_LENGTH = 0.5;

const QString MeshPolygon::BOUNDARY_POLYGON_FILENAME = "Main";

MeshPolygon::MeshPolygon() {}

MeshPolygon::MeshPolygon(const QString &filename, MeshPolygonType meshPolygonType) : filename(filename), meshPolygonType(meshPolygonType) {}

MeshPolygon& MeshPolygon::operator=(const MeshPolygon &meshPolygon) {
    this->filename = meshPolygon.getFilename();
    this->meshPolygonType = meshPolygon.getMeshPolygonType();
    this->minimumAngle = meshPolygon.getMinimumAngle();
    this->maximumEdgeLength = meshPolygon.getMaximumEdgeLength();

    return *this;
}

bool MeshPolygon::operator==(const MeshPolygon &meshPolygon) {
    return this->filename == meshPolygon.getFilename() && this->meshPolygonType == meshPolygon.getMeshPolygonType();
}

void MeshPolygon::setFilename(const QString &filename) {
    this->filename = filename;
}

QString MeshPolygon::getFilename() const {
    return filename;
}

MeshPolygon::MeshPolygonType MeshPolygon::getMeshPolygonType() const {
    return meshPolygonType;
}

void MeshPolygon::setMinimumAngle(const double &minimumAngle) {
    this->minimumAngle = minimumAngle;
}

double MeshPolygon::getMinimumAngle() const {
    return minimumAngle;
}

double MeshPolygon::getMinimumAngleInCGALRepresentation() const {
    // http://doc.cgal.org/latest/Mesh_2/classCGAL_1_1Delaunay__mesh__size__criteria__2.html
    return qPow(sin(minimumAngle * M_PI / 180), 2.0);
}

void MeshPolygon::setMaximumEdgeLength(const double &maximumEdgeLength) {
    this->maximumEdgeLength = maximumEdgeLength;
}

double MeshPolygon::getMaximumEdgeLength() const {
    return maximumEdgeLength;
}

void MeshPolygon::setOptimalParameters() {
    double smallEdgeLength = qMin(width(), height()) / 10.0;
    double optimalEdgeLength = qMax(smallEdgeLength, DEFAULT_MAXIMUM_EDGE_LENGTH);

    this->maximumEdgeLength = optimalEdgeLength;
    this->minimumAngle = DEFAULT_MINIMUM_ANGLE;
}

double MeshPolygon::width() const {
    if (right_vertex() == this->vertices_end() || left_vertex() == this->vertices_end()) {
        return 0.0;
    }

    return right_vertex()->x() - left_vertex()->x();
}

double MeshPolygon::height() const {
    if (top_vertex() == this->vertices_end() || bottom_vertex() == this->vertices_end()) {
        return 0.0;
    }

    return top_vertex()->y() - bottom_vertex()->y();
}
