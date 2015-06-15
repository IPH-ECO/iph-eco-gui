#include "include/domain/project.h"

Project::Project(const QString &name, const QString &description, const bool &hydrodynamic, const bool &sediment, const bool &waterQuality) :
        id(0), name(name), description(description), hydrodynamic(hydrodynamic), waterQuality(waterQuality),
        sediment(sediment), dirty(false)
{}

void Project::setId(const uint &id) {
    if (!isPersisted()) {
        this->id = id;
    }
}

uint Project::getId() const {
    return id;
}

void Project::setName(const QString &name) {
    this->name = name;
    this->setDirty(true);
}

QString Project::getName() const {
    return this->name;
}

void Project::setDescription(const QString &description) {
    this->description = description;
    this->setDirty(true);
}

QString Project::getDescription() const {
    return this->description;
}

void Project::setFilename(const QString &filename) {
    this->filename = filename;
    this->setDirty(true);
}

QString Project::getFilename() const {
    return this->filename;
}

void Project::setHydrodynamic(const bool &hydrodynamic) {
    this->hydrodynamic = hydrodynamic;
    this->setDirty(true);
}

bool Project::getHydrodynamic() const {
    return this->hydrodynamic;
}

void Project::setWaterQuality(const bool &waterQuality) {
    this->waterQuality = waterQuality;
    this->setDirty(true);
}

bool Project::getWaterQuality() const {
    return waterQuality;
}

void Project::setSediment(const bool &sediment) {
    this->sediment = sediment;
    this->setDirty(true);
}

bool Project::getSediment() const {
    return sediment;
}

QSet<Mesh*> Project::getMeshes() const {
    return meshes;
}

bool Project::addMesh(Mesh *mesh) {
    if (meshes.contains(mesh)) {
        return false;
    }
    
    meshes.insert(mesh);
    
    return true;
}

void Project::removeMesh(Mesh *mesh) {
    meshes.remove(mesh);
    delete mesh;
    this->setDirty(true);
}

bool Project::containsMesh(const QString &meshName) {
    return this->getMesh(meshName) != NULL;
}

Mesh* Project::getMesh(const QString &meshName) const {
    for (QSet<Mesh*>::const_iterator it = meshes.begin(); it != meshes.end(); it++) {
        if ((*it)->getName() == meshName) {
            return *it;
        }
    }

    return NULL;
}

Mesh* Project::getMesh(const uint &id) const {
    for (QSet<Mesh*>::const_iterator it = meshes.begin(); it != meshes.end(); it++) {
        if ((*it)->getId() == id) {
            return *it;
        }
    }
    
    return NULL;
}

bool Project::addGridDataConfiguration(GridDataConfiguration *gridDataConfiguration) {
    if (this->gridDataConfigurations.contains(gridDataConfiguration)) {
        return false;
    }

    this->gridDataConfigurations.insert(gridDataConfiguration);
    this->setDirty(true);

    return true;
}

void Project::removeGridDataConfiguration(const QString &configurationName) {
    GridDataConfiguration *gridDataConfiguration = getGridDataConfiguration(configurationName);

    if (gridDataConfiguration != NULL) {
        gridDataConfigurations.remove(gridDataConfiguration);
        delete gridDataConfiguration;
    }

    this->setDirty(true);
}

GridDataConfiguration* Project::getGridDataConfiguration(const QString &configurationName) {
    for (QSet<GridDataConfiguration*>::const_iterator it = gridDataConfigurations.begin(); it != gridDataConfigurations.end(); it++) {
        if ((*it)->getName() == configurationName) {
            return *it;
        }
    }

    return NULL;
}

QSet<GridDataConfiguration*> Project::getGridDataConfigurations() const {
    return gridDataConfigurations;
}

bool Project::isPersisted() const {
    return this->id != 0;
}

void Project::setDirty(const bool &dirty) {
    this->dirty = dirty;
}

bool Project::isDirty() const {
    return this->dirty;
}
