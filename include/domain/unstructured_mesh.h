#ifndef UNSTRUCTURED_MESH_H
#define UNSTRUCTURED_MESH_H

#include "include/exceptions/mesh_exception.h"
#include "mesh.h"
#include "include/utility/cgal_definitions.h"

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
    void clearCDT();

    virtual bool isGenerated();
    virtual bool instanceOf(const QString &type);
    virtual void buildDomain(const QString &filename);
    virtual void clear();
};

#endif // UNSTRUCTURED_MESH_H
