#ifndef STRUCTURED_MESH_H
#define STRUCTURED_MESH_H

#include "mesh.h"

class StructuredMesh : public Mesh {
private:
    uint resolution;

public:
    StructuredMesh();
    StructuredMesh(QString &name);

    uint getResolution() const;
    void setResolution(const uint &resolution);

    void generate();

    virtual bool isGenerated();
    virtual void buildDomain(const QString &filename);
    virtual void clear();
};

#endif // STRUCTURED_MESH_H
