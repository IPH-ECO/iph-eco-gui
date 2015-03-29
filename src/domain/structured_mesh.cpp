#include "include/domain/structured_mesh.h"

StructuredMesh::StructuredMesh(QString &_name) : Mesh(_name), rows(1), columns(1) {}

StructuredMesh::StructuredMesh(QString &_name, uint &_rows, uint &_columns) : Mesh(_name), rows(_rows), columns(_columns) {}

void StructuredMesh::setRows(const uint rows) {
    this->rows = rows;
}

uint StructuredMesh::getRows() const {
    return this->rows;
}

void StructuredMesh::setColumns(const uint columns) {
    this->columns = columns;
}

uint StructuredMesh::getColumns() const {
    return this->columns;
}

