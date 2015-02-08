#include "include/services/mesh_service.h"

MeshService::MeshService() {
    meshManager = MeshManager::getInstance();
}

void MeshService::addStructuredMesh(QString &meshName) {
    if (meshManager->meshExists(MeshManager::STRUCTURED, meshName)) {
        throw MeshException(QString("Mesh '%1' already added.").arg(meshName));
    }

    meshManager->addMesh(MeshManager::STRUCTURED, meshName);
}

void MeshService::addUnstructuredMesh(QString &meshName) {
    if (meshManager->meshExists(MeshManager::UNSTRUCTURED, meshName)) {
        throw MeshException(QString("Mesh '%1' already added.").arg(meshName));
    }

    meshManager->addMesh(MeshManager::UNSTRUCTURED, meshName);
}

void MeshService::setStructuredMeshBoundaryFile(QString &meshName, QString &boundaryFilePath) {
    Mesh *mesh = meshManager->getMesh(MeshManager::STRUCTURED, meshName);
    mesh->setBoundaryFilePath(boundaryFilePath);
}

void MeshService::setUnstructuredMeshBoundaryFile(QString &meshName, QString &boundaryFilePath) {
    Mesh *mesh = meshManager->getMesh(MeshManager::UNSTRUCTURED, meshName);
    mesh->setBoundaryFilePath(boundaryFilePath);
}

void MeshService::removeStructuredMesh(QString &meshName) {
    meshManager->removeMesh(MeshManager::STRUCTURED, meshName);
}

void MeshService::removeUnstructuredMesh(QString &meshName) {
    meshManager->removeMesh(MeshManager::UNSTRUCTURED, meshName);
}

bool MeshService::structuredMeshExists(QString &meshName) {
    return meshManager->meshExists(MeshManager::STRUCTURED, meshName);
}

bool MeshService::unstructuredMeshExists(QString &meshName) {
    return meshManager->meshExists(MeshManager::UNSTRUCTURED, meshName);
}

QJsonObject MeshService::getUnstructuredMeshBoundary(QString &meshName, QProgressDialog *progressDialog) {
    Mesh *mesh = meshManager->getMesh(MeshManager::UNSTRUCTURED, meshName);
    MeshObserver observer(meshManager);

    observer.setProgressDialog(progressDialog);

    return meshManager->getBoundaryJson(mesh);
}

void MeshService::generateUnstructuredMesh(QString &meshName) {
}

void MeshService::generateStructuredMesh(QString &meshName) {
}
