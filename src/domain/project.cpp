#include "include/domain/project.h"
Project::Project(QString &_name, QString &_description, bool &_hydrodynamic, bool &_sediment, bool &_waterQuality) :
    name(_name), description(_description), hydrodynamic(_hydrodynamic), waterQuality(_waterQuality),
    sediment(_sediment), meshes(QSet<Mesh*>())
{}

void Project::setId(const qint8 &id) {
    this->id = id;
}

qint8 Project::getId() const {
    return this->id;
}

void Project::setName(const QString &name) {
    this->name = name;
}

QString Project::getName() const {
    return this->name;
}

void Project::setDescription(const QString &description) {
    this->description = description;
}

QString Project::getDescription() const {
    return this->description;
}

void Project::setFilename(const QString &filename) {
    this->filename = filename;
}

QString Project::getFilename() const {
    return this->filename;
}

void Project::setHydrodynamic(const bool &hydrodynamic) {
    this->hydrodynamic = hydrodynamic;
}

bool Project::getHydrodynamic() const {
    return this->hydrodynamic;
}

void Project::setWaterQuality(const bool &waterQuality) {
    this->waterQuality = waterQuality;
}

bool Project::getWaterQuality() const {
    return waterQuality;
}

void Project::setSediment(const bool &sediment) {
    this->sediment = sediment;
}

bool Project::getSediment() const {
    return sediment;
}

const QSet<Mesh*> Project::getMeshes() {
    return meshes;
}

bool Project::addMesh(Mesh *mesh) {
    if (!containsMesh(mesh)) {
        this->meshes.insert(mesh);
        return true;
    }
    return false;
}

void Project::removeMesh(Mesh *mesh) {
    Mesh *m = getMesh(mesh);

    this->meshes.remove(m);
    delete m;
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
