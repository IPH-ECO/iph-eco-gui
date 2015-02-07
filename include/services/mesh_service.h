#ifndef GRIDSERVICE_H
#define GRIDSERVICE_H

#include <QString>
#include <QStringList>

#include "include/domain/mesh_manager.h"
#include "include/exceptions/mesh_exception.h"

class MeshService {
private:
    MeshManager *meshManager;

public:
    MeshService();

    void addStructuredMesh(QString &name);
    void addUnstructuredMesh(QString &name);

    void setStructuredMeshBoundaryFile(QString &meshName, QString &boundaryFilePath);
    void setUnstructuredMeshBoundaryFile(QString &meshName, QString &boundaryFilePath);

    void removeStructuredMesh(QString &name);
    void removeUnstructuredMesh(QString &name);

    bool structuredMeshExists(QString &name);
    bool unstructuredMeshExists(QString &name);
};

#endif // GRIDSERVICE_H
