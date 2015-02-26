#include "include/domain/unstructured_mesh.h"

UnstructuredMesh::UnstructuredMesh() {}

UnstructuredMesh::UnstructuredMesh(QString &_name) : Mesh(_name), minimumAngle(0.125), maximumEdgeLength(0.5) {}

UnstructuredMesh::UnstructuredMesh(QString &_name, QString &_boundaryFilePath) : Mesh(_name, _boundaryFilePath) {}

UnstructuredMesh::UnstructuredMesh(QString &_name, QString &_boundaryFilePath, double &_minimumAngle, double &_maximumEdgeLength) :
    Mesh(_name, _boundaryFilePath), minimumAngle(_minimumAngle), maximumEdgeLength(_maximumEdgeLength)
{}

void UnstructuredMesh::setMinimumAngle(const double &minimumAngle) {
    this->minimumAngle = minimumAngle;
}

double UnstructuredMesh::getMinimumAngle() const {
    return minimumAngle;
}

void UnstructuredMesh::setMaximumEdgeLength(const double &maximumEdgeLength) {
    this->maximumEdgeLength = maximumEdgeLength;
}

double UnstructuredMesh::getMaximumEdgeLength() const {
    return maximumEdgeLength;
}

void UnstructuredMesh::generate() {
    const MeshPolygon *boundaryPolygon = getBoundaryPolygon();

    if (boundaryPolygon == NULL) {
        throw MeshException("No boundary vertices found.");
    }

    if (cdt.number_of_vertices() > 0) {
        return; // Mesh already generated
    }

    cdt.insert(boundaryPolygon->vertices_begin(), boundaryPolygon->vertices_end());

    for (unsigned long i = 0; i < boundaryPolygon->size(); i++) {
        Point p1 = (*boundaryPolygon)[i];
        Point p2 = (*boundaryPolygon)[i == boundaryPolygon->size() - 1 ? 0 : i + 1];

        cdt.insert_constraint(p1, p2);
    }

    Criteria criteria(minimumAngle, maximumEdgeLength);
    Mesher mesher(cdt, criteria);

    mesher.refine_mesh();
}

const CDT* UnstructuredMesh::getCDT() {
    return &cdt;
}

void UnstructuredMesh::buildDomain() {
    cdt.clear();
    Mesh::buildDomain();
}

void UnstructuredMesh::clear() {
    Mesh::clear();
    cdt.clear();
    minimumAngle = 0.125;
    maximumEdgeLength = 0.5;
}
