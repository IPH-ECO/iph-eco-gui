#include "include/domain/mesh_polygon.h"

MeshPolygon::MeshPolygon(bool hole) : hole(hole) {}

void MeshPolygon::setHole(const bool &hole) {
    this->hole = hole;
}

bool MeshPolygon::isHole() const {
    return hole;
}
