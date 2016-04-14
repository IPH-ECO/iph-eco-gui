#ifndef STRUCTURED_MESH_H
#define STRUCTURED_MESH_H

#include "simulation_data_type.h"
#include "mesh.h"
#include "hydrodynamic_configuration.h"

class StructuredMesh : public Mesh {
private:
    double resolution;

    bool pointInMesh(double *point);

public:
    StructuredMesh();

    double getResolution() const;
    void setResolution(double resolution);
    void computeBounds(ulong *points);

    virtual bool instanceOf(const QString &type);
    virtual void generate();
    
    SimulationDataType::StructuredMesh* toSimulationDataType(const HydrodynamicConfiguration *hydrodynamicConfiguration) const;
};

#endif // STRUCTURED_MESH_H
