#include "include/domain/project.h"

Project::Project(QString &name, QString &description, QString &analysis) {
    this->name = name;
    this->description = description;
    this->analysis = analysis;
    this->version = 1;
}

void Project::setName(const QString &name) {
    this->name = name;
}

const QString Project::getName() const {
    return this->name;
}

void Project::setVersion(const int &version) {
    this->version = version;
}

int Project::getVersion() const {
    return this->version;
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

long Project::getSelectedGridDataId() const {
    return selectedGridDataId;
}

void Project::setSelectedHydrodynamicDataId(const long &selectedHydrodynamicDataId) {
    this->selectedHydrodynamicDataId = selectedHydrodynamicDataId;
}

long Project::getSelectedHydrodynamicDataId() const {
    return this->selectedHydrodynamicDataId;
}

void Project::setSelectedWaterQualityDataId(const long &selectedWaterQualityDataId) {
    this->selectedWaterQualityDataId = selectedWaterQualityDataId;
}

long Project::getSelectedWaterQualityDataId() const {
    return selectedWaterQualityDataId;
}

void Project::setSelectedSedimentDataId(const long &selectedSedimentDataId) {
    this->selectedSedimentDataId = selectedSedimentDataId;
}

long Project::getSelectedSedimentDataId() const {
    return selectedSedimentDataId;
}

void Project::setSelectedMeteorologicalDataId(const long &selectedMeteorologicalDataId) {
    this->selectedMeteorologicalDataId = selectedMeteorologicalDataId;
}

long Project::getSelectedMeteorologicalDataId() const {
    return this->selectedMeteorologicalDataId;
}
