#include "include/domain/structured_mesh.h"

StructuredMesh::StructuredMesh() : width(1), height(1) {}

StructuredMesh::StructuredMesh(QString &name) : Mesh(name), width(1), height(1) {}

uint StructuredMesh::getWidth() const {
    return width;
}

void StructuredMesh::setWidth(const uint &width) {
    this->width = width;
}

uint StructuredMesh::getHeight() const {
    return height;
}

void StructuredMesh::setHeight(const uint &height) {
    this->height = height;
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
