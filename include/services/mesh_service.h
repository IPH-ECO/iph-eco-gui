#ifndef MESH_SERVICE_H
#define MESH_SERVICE_H

#include <QString>
#include <QJsonObject>
#include <QProgressDialog>

#include "include/domain/managers/mesh_manager.h"

class MeshService {
protected:
    MeshManager *meshManager;

    MeshService() {
        this->meshManager = MeshManager::getInstance();
    }

public:
    virtual void addMesh(QString &meshName) = 0;
    virtual void removeMesh(QString &meshName) = 0;
    virtual void setBoundaryFilePath(QString &meshName, QString &boundaryFilePath) = 0;
    virtual bool meshExists(QString &meshName) = 0;
    virtual QJsonObject getBoundaryJson(QString &meshName, QProgressDialog *progressDialog) = 0;
    virtual ~MeshService() {}
};

#endif // MESH_SERVICE_H
