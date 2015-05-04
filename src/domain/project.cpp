#include "include/domain/project.h"
Project::Project(QString &_name, QString &_description, bool &_hydrodynamic, bool &_sediment, bool &_waterQuality) :
        name(_name), description(_description), hydrodynamic(_hydrodynamic), waterQuality(_waterQuality),
        sediment(_sediment), meshes(QSet<Mesh*>()) {
    this->dirty = false;
}

void Project::setId(const qint8 &id) {
    this->id = id;
    this->setDirty(true);
}

qint8 Project::getId() const {
    return this->id;
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

const QSet<Mesh*>& Project::getMeshes() {
    return meshes;
}

bool Project::addMesh(Mesh *mesh) {
    if (!containsMesh(mesh)) {
        this->meshes.insert(mesh);
        this->setDirty(true);
        return true;
    }
    return false;
}

void Project::removeMesh(Mesh *mesh) {
    Mesh *m = getMesh(mesh);

    this->meshes.remove(m);
    delete m;
    this->setDirty(true);
}

bool Project::containsMesh(Mesh *mesh) {
    return this->meshes.contains(mesh);
}

Mesh* Project::getMesh(Mesh *mesh) {
    for (QSet<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++) {
        if ((*it)->getName() == mesh->getName() && typeid(*it) == typeid(mesh)) {
            return *it;
        }
    }

    return NULL;
}

Mesh* Project::getMesh(const QString &meshName) {
    for (QSet<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++) {
        if ((*it)->getName() == meshName) {
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

QSet<GridDataConfiguration*>& Project::getGridDataConfigurations() {
    return gridDataConfigurations;
}

void Project::setDirty(const bool &dirty) {
    this->dirty = dirty;
}

bool Project::isDirty() const {
    return this->dirty;
}
