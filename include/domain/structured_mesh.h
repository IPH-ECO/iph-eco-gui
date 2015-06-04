#ifndef STRUCTURED_MESH_H
#define STRUCTURED_MESH_H

#include <vtkStructuredGrid.h>

#include "include/utility/cgal_definitions.h"
#include "quad.h"
#include "mesh.h"

class StructuredMesh : public Mesh {
private:
    uint resolution;
    vtkSmartPointer<vtkStructuredGrid> grid;

    void computeBounds(ulong *points);

public:
    StructuredMesh();
    StructuredMesh(QString &name);
    StructuredMesh(QString &name, uint &resolution);

    uint getResolution() const;
    void setResolution(const uint &resolution);
    vtkStructuredGrid* getGrid();

    void generate();
    void clearGrid();

    virtual bool isGenerated();
    virtual bool instanceOf(const QString &type);
    virtual void buildDomain(const QString &filename);
    virtual void clear();
};

#endif // STRUCTURED_MESH_H
