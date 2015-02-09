#ifndef UNSTRUCTURED_MESHSERVICE_H
#define UNSTRUCTURED_MESHSERVICE_H

#include "mesh_service.h"
#include "include/domain/observers/mesh_observer.h"

class UnstructuredMeshService : public MeshService {
public:
    virtual void addMesh(QString &meshName);
    virtual void removeMesh(QString &meshName);
    virtual void setBoundaryFilePath(QString &meshName, QString &boundaryFilePath);
    virtual bool meshExists(QString &meshName);
    virtual QJsonObject getBoundaryJson(QString &meshName, QProgressDialog *progressDialog);
};

#endif // UNSTRUCTURED_MESHSERVICE_H
