#include "include/domain/mesh_polygon.h"

MeshPolygon::MeshPolygon(bool hole) : hole(hole) {}

void MeshPolygon::setHole(const bool &hole) {
    this->hole = hole;
}

bool MeshPolygon::isHole() const {
    return hole;
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
