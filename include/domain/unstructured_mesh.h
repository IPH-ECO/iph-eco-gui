#ifndef UNSTRUCTURED_MESH_H
#define UNSTRUCTURED_MESH_H

#include "include/exceptions/mesh_exception.h"
#include "mesh.h"
#include "include/utility/cgal_definitions.h"

#include <QList>

class UnstructuredMesh : public Mesh {
private:
    vtkSmartPointer<vtkPolyData> polyData;

    void mark_domains(CDT &cdt, CDT::Face_handle start, int index, QList<CDT::Edge>& border);
    void mark_domains(CDT &cdt);

public:
    UnstructuredMesh();

    virtual bool instanceOf(const QString &type);
    virtual void generate();
    virtual vtkPolyData* getGrid();
};

#endif // UNSTRUCTURED_MESH_H
