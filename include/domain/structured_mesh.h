#ifndef STRUCTUREDMESH_H
#define STRUCTUREDMESH_H

#include "mesh.h"

class StructuredMesh : public Mesh {
private:
    uint rows;
    uint columns;
public:
    StructuredMesh(QString &_name);
    StructuredMesh(QString &_name, uint &_rows, uint &_columns);

    void setRows(const uint rows);
    uint getRows() const;

    void setColumns(const uint columns);
    uint getColumns() const;
};

#endif // STRUCTUREDMESH_H
