#include "include/domain/mesh_manager.h"

MeshManager* MeshManager::instance = NULL;

MeshManager::MeshManager() : structuredMeshes(QList<StructuredMesh>()), unstructuredMeshes(QList<UnstructuredMesh>()) {}

MeshManager* MeshManager::getInstance() {
    if (instance == NULL) {
        instance = new MeshManager();
    }

    return instance;
}

void MeshManager::addMesh(MeshType meshType, QString &meshName) {
    if (meshType == STRUCTURED) {
        this->structuredMeshes.push_back(StructuredMesh(meshName));
    } else {
        this->unstructuredMeshes.push_back(UnstructuredMesh(meshName));
    }
}

void MeshManager::removeMesh(MeshType meshType, QString &meshName) {
    if (meshType == STRUCTURED) {
        this->structuredMeshes.removeOne(StructuredMesh(meshName));
    } else {
        this->unstructuredMeshes.removeOne(UnstructuredMesh(meshName));
    }
}

bool MeshManager::meshExists(MeshType meshType, QString &meshName) {
    if (meshType == STRUCTURED) {
        return this->structuredMeshes.contains(StructuredMesh(meshName));
    }

    return this->unstructuredMeshes.contains(UnstructuredMesh(meshName));
}

Mesh MeshManager::getMesh(MeshType meshType, QString &meshName) {
    if (meshType == STRUCTURED) {
        return *std::find(this->structuredMeshes.begin(), this->structuredMeshes.end(), StructuredMesh(meshName));
    }

    return *std::find(this->unstructuredMeshes.begin(), this->unstructuredMeshes.end(), UnstructuredMesh(meshName));
}
