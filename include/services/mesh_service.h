#ifndef GRIDSERVICE_H
#define GRIDSERVICE_H

#include <QString>
#include <QJsonObject>
#include <QProgressDialog>

#include "include/domain/managers/mesh_manager.h"
#include "include/exceptions/mesh_exception.h"
#include "include/domain/observers/mesh_observer.h"

class MeshService {
private:
    MeshManager *meshManager;

public:
    MeshService();

    void addStructuredMesh(QString &meshName);
    void addUnstructuredMesh(QString &meshName);

    void setStructuredMeshBoundaryFile(QString &meshName, QString &boundaryFilePath);
    void setUnstructuredMeshBoundaryFile(QString &meshName, QString &boundaryFilePath);

    void removeStructuredMesh(QString &meshName);
    void removeUnstructuredMesh(QString &meshName);

    bool structuredMeshExists(QString &meshName);
    bool unstructuredMeshExists(QString &meshName);

    QJsonObject getUnstructuredMeshBoundary(QString &meshName, QProgressDialog *progressDialog);

    void generateUnstructuredMesh(QString &meshName);
    void generateStructuredMesh(QString &meshName);
};

#endif // GRIDSERVICE_H
