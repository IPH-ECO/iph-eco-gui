#ifndef STRUCTURED_MESH_H
#define STRUCTURED_MESH_H

#include "simulation_data_type.h"
#include "mesh.h"
#include "hydrodynamic_configuration.h"

class StructuredMesh : public Mesh {
private:
    uint resolution;

    bool pointInMesh(double *point);

public:
    StructuredMesh();

    uint getResolution() const;
    void setResolution(const uint &resolution);
    void computeBounds(ulong *points);

    virtual bool instanceOf(const QString &type);
    virtual void generate();
    
    SimulationDataType::StructuredMesh* toSimulationDataType(const HydrodynamicConfiguration *hydrodynamicConfiguration) const;
};

#endif // STRUCTURED_MESH_H
