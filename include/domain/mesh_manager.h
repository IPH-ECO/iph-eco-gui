#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include "include/domain/mesh.h"
#include "include/domain/structured_mesh.h"
#include "include/domain/unstructured_mesh.h"

class MeshManager {
private:
    static MeshManager *instance;

    QList<StructuredMesh> structuredMeshes;
    QList<UnstructuredMesh> unstructuredMeshes;
    bool showUTMCoordinates;
    bool showVertexesLabels;
    bool showTrianglesLabels;
    bool showEdgesLabels;

    MeshManager();
public:
    enum MeshType { STRUCTURED, UNSTRUCTURED };

    static MeshManager* getInstance();

    void addMesh(MeshType meshType, QString &meshName);
    void removeMesh(MeshType meshType, QString &meshName);
    bool meshExists(MeshType meshType, QString &meshName);
    Mesh getMesh(MeshType meshType, QString &meshName);
    void toogleUTMCoordinates(MeshType meshType, bool &show);
    void toogleVertexesLabels(MeshType meshType, bool &show);
};

#endif // MESHMANAGER_H