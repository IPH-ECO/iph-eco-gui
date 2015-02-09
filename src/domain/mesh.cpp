#include "include/domain/mesh.h"

Mesh::Mesh(QString &_name) : name(_name) {}

Mesh::Mesh(QString &_name, QString &_boundaryFilePath) : name(_name), boundaryFilePath(_boundaryFilePath) {}

void Mesh::setName(const QString &name) {
    this->name = name;
}

QString Mesh::getName() const {
    return this->name;
}

void Mesh::setBoundaryFilePath(const QString &boundaryFilePath) {
    this->boundaryFilePath = boundaryFilePath;
}

QString Mesh::getBoundaryFilePath() const {
    return this->boundaryFilePath;
}
