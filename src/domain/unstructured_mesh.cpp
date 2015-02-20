#include "include/domain/unstructured_mesh.h"

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
