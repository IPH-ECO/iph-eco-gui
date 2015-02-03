#include "include/domain/project.h"

Project::Project(QString &_name, QString &_description, bool &_hydrodynamic, bool &_sediment, bool &_waterQuality) :
    name(_name), description(_description), hydrodynamic(_hydrodynamic), waterQuality(_waterQuality),
    sediment(_sediment), selectedMeteorologicalDataId(0), version(1)
{}

void Project::setId(const qlonglong &id) {
    this->id = id;
}

qlonglong Project::getId() const {
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

void Project::setBoundaryData(const QString &boundaryData) {
    this->boundaryData = boundaryData;
}

QString Project::getBoundaryData() const {
    return this->boundaryData;
}

void Project::setSelectedGridDataId(const int &selectedGridDataId) {
    this->selectedGridDataId = selectedGridDataId;
}

qlonglong Project::getSelectedGridDataId() const {
    return selectedGridDataId;
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

void Project::setSelectedMeteorologicalDataId(const qlonglong &selectedMeteorologicalDataId) {
    this->selectedMeteorologicalDataId = selectedMeteorologicalDataId;
}

qlonglong Project::getSelectedMeteorologicalDataId() const {
    return this->selectedMeteorologicalDataId;
}

void Project::setVersion(const int &version) {
    this->version = version;
}

int Project::getVersion() const {
    return this->version;
}
