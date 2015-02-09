#include "include/services/unstructured_mesh_service.h"

void UnstructuredMeshService::addMesh(QString &meshName) {
    if (meshManager->meshExists(MeshManager::UNSTRUCTURED, meshName)) {
        throw MeshException(QString("Mesh '%1' already added.").arg(meshName));
    }

    meshManager->addMesh(MeshManager::UNSTRUCTURED, meshName);
}

void UnstructuredMeshService::removeMesh(QString &meshName) {
    meshManager->removeMesh(MeshManager::UNSTRUCTURED, meshName);
}

void UnstructuredMeshService::setBoundaryFilePath(QString &meshName, QString &boundaryFilePath) {
    Mesh *mesh = meshManager->getMesh(MeshManager::UNSTRUCTURED, meshName);
    mesh->setBoundaryFilePath(boundaryFilePath);
}

bool UnstructuredMeshService::meshExists(QString &meshName) {
    return meshManager->meshExists(MeshManager::UNSTRUCTURED, meshName);
}

QJsonObject UnstructuredMeshService::getBoundaryJson(QString &meshName, QProgressDialog *progressDialog) {
    Mesh *mesh = meshManager->getMesh(MeshManager::UNSTRUCTURED, meshName);
    MeshObserver observer(meshManager);

    observer.setProgressDialog(progressDialog);

    return meshManager->getBoundaryJson(mesh);
}
