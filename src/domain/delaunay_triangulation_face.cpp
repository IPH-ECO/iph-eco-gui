#include "include/domain/delaunay_triangulation_face.h"

DelaunayTriangulationFace::DelaunayTriangulationFace() {}

DelaunayTriangulationFace::~DelaunayTriangulationFace() {}

int DelaunayTriangulationFace::getNestingLevel() const {
    return nestingLevel;
}

void DelaunayTriangulationFace::setNestingLevel(const int nestingLevel) {
    this->nestingLevel = nestingLevel;
}

bool DelaunayTriangulationFace::isInDomain() {
    return nestingLevel % 2 == 1; //positive odd
}