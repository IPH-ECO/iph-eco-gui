#ifndef UNSTRUCTURED_MESH_H
#define UNSTRUCTURED_MESH_H

#include "simulation_data_type.h"
#include "mesh.h"
#include <exceptions/mesh_exception.h>

#ifdef WITH_UNSTRUCTURED_MESH
#include <utility/cgal_definitions.h>
#endif

#include <QList>

class UnstructuredMesh : public Mesh {
private:
    void mark_domains(CDT &cdt, CDT::Face_handle start, int index, QList<CDT::Edge>& border);
    void mark_domains(CDT &cdt);

public:
    UnstructuredMesh();

    virtual bool instanceOf(const QString &type);
    virtual void generate();
    
    SimulationDataType::UnstructuredMesh* toSimulationDataType() const;
};

#endif // UNSTRUCTURED_MESH_H
