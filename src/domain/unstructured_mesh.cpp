#include "include/domain/unstructured_mesh.h"

UnstructuredMesh::UnstructuredMesh(QString &_name) : Mesh(_name), maximumAngle(0.125), minimumEdgeLength(0.5) {}

UnstructuredMesh::UnstructuredMesh(QString &_name, QString &_boundaryFilePath, double &_maximumAngle, double &_minimumEdgeLength) :
    Mesh(_name, _boundaryFilePath), maximumAngle(_maximumAngle), minimumEdgeLength(_minimumEdgeLength)
{}

void UnstructuredMesh::setMaximumAngle(const double &maximumAngle) {
    this->maximumAngle = maximumAngle;
}

double UnstructuredMesh::getMaximumAngle() const {
    return maximumAngle;
}

void UnstructuredMesh::setMinimumEdgeLength(const double &minimumEdgeLength) {
    this->minimumEdgeLength = minimumEdgeLength;
}

double UnstructuredMesh::getMinimumEdgeLength() const {
    return minimumEdgeLength;
}
