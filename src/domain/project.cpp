#include "include/domain/project.h"

Project::Project(QString &_name, QString &_description, QString &_analysis) :
    name(_name), description(_description), analysis(_analysis),
    selectedGridDataId(0), selectedHydrodynamicDataId(0), selectedWaterQualityDataId(0),
    selectedSedimentDataId(0), selectedMeteorologicalDataId(0), version(1)
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

void Project::setAnalysis(const QString &analysis) {
    this->analysis = analysis;
}

QString Project::getAnalysis() const {
    return this->analysis;
}

void Project::setSelectedGridDataId(const int &selectedGridDataId) {
    this->selectedGridDataId = selectedGridDataId;
}

qlonglong Project::getSelectedGridDataId() const {
    return selectedGridDataId;
}

void Project::setSelectedHydrodynamicDataId(const qlonglong &selectedHydrodynamicDataId) {
    this->selectedHydrodynamicDataId = selectedHydrodynamicDataId;
}

qlonglong Project::getSelectedHydrodynamicDataId() const {
    return this->selectedHydrodynamicDataId;
}

void Project::setSelectedWaterQualityDataId(const qlonglong &selectedWaterQualityDataId) {
    this->selectedWaterQualityDataId = selectedWaterQualityDataId;
}

qlonglong Project::getSelectedWaterQualityDataId() const {
    return selectedWaterQualityDataId;
}

void Project::setSelectedSedimentDataId(const qlonglong &selectedSedimentDataId) {
    this->selectedSedimentDataId = selectedSedimentDataId;
}

qlonglong Project::getSelectedSedimentDataId() const {
    return selectedSedimentDataId;
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
