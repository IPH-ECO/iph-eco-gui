#ifndef UNSTRUCTUREDMESH_H
#define UNSTRUCTUREDMESH_H

#include "include/utility/delaunay_triangulation_definitions.h"
#include "include/exceptions/mesh_exception.h"
#include "mesh.h"

#include <QList>

class UnstructuredMesh : public Mesh {
private:
    CDT cdt;

    void mark_domains(CDT::Face_handle start, int index, QList<CDT::Edge>& border);
    void mark_domains();

public:
    UnstructuredMesh();
    UnstructuredMesh(QString &_name);

    void generate();

    const CDT* getCDT();

    virtual void buildDomain(const QString &filename);
    virtual void clear();
};

#endif // UNSTRUCTUREDMESH_H
