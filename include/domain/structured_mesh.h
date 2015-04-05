#ifndef STRUCTUREDMESH_H
#define STRUCTUREDMESH_H

#include "mesh.h"

class StructuredMesh : public Mesh {
private:
    uint width;
    uint height;
public:
    StructuredMesh();
    StructuredMesh(QString &name);

    uint getWidth() const;
    void setWidth(const uint &width);
    uint getHeight() const;
    void setHeight(const uint &height);

    void generate();

    virtual bool isGenerated();
    virtual void buildDomain(const QString &filename);
    virtual void clear();
};

#endif // STRUCTUREDMESH_H
