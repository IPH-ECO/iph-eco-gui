#include "include/domain/structured_mesh.h"

StructuredMesh::StructuredMesh() : resolution(1) {}

StructuredMesh::StructuredMesh(QString &name) : Mesh(name), resolution(1) {}

uint StructuredMesh::getResolution() const {
    return resolution;
}

void StructuredMesh::setResolution(const uint &resolution) {
    this->resolution = resolution;
}

void StructuredMesh::generate() {

}

bool StructuredMesh::isGenerated() {
    return false;
}

void StructuredMesh::buildDomain(const QString &filename) {
    Mesh::buildDomain(filename);
}

void StructuredMesh::clear() {

}
