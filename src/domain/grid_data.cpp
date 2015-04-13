#include "include/domain/grid_data.h"

GridData::GridData() : mesh(NULL) {}

GridData::GridData(Mesh *mesh) : mesh(mesh) {}

QString GridData::getName() const {
    return name;
}

void GridData::setName(const QString &name) {
    this->name = name;
}

Mesh* GridData::getMesh() const {
    return mesh;
}

void GridData::setMesh(Mesh *mesh) {
    this->mesh = mesh;
}
