#include "delaunay_triangulation_face.h"

DelaunayTriangulationFace::DelaunayTriangulationFace() {}
DelaunayTriangulationFace::~DelaunayTriangulationFace() {
    for (QSet<CellInfo*>::iterator it = cellInfoSet.begin(); it != cellInfoSet.end(); it++) {
        delete *it;
    }
}

int DelaunayTriangulationFace::getNestingLevel() const {
    return nestingLevel;
}

void DelaunayTriangulationFace::setNestingLevel(const int nestingLevel) {
    this->nestingLevel = nestingLevel;
}

bool DelaunayTriangulationFace::isInDomain() {
    return nestingLevel % 2 == 1; //positive odd
}

CellInfo* DelaunayTriangulationFace::getCellInfo(GridInformationType &gridInformationType) {
    for (QSet<CellInfo*>::const_iterator it = cellInfoSet.begin(); it != cellInfoSet.end(); it++) {
        if ((*it)->getGridInformationType() == gridInformationType) {
            return *it;
        }
    }

    return NULL;
}

void DelaunayTriangulationFace::addCellInfo(CellInfo *cellInfo) {
    cellInfoSet.insert(cellInfo);
}

QSet<CellInfo*>& DelaunayTriangulationFace::getCellInfoSet() {
    return cellInfoSet;
}
