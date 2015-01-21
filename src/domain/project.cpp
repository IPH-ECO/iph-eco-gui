#include "../../include/domain/project.h"

IPH::Domain::Project::Project() {

}

void IPH::Domain::Project::setName(const std::string &name) {
    this->name = name;
}

std::string IPH::Domain::Project::getName() const {
    return this->name;
}

void IPH::Domain::Project::setVersion(const int &version) {
    this->version = version;
}

int IPH::Domain::Project::getVersion() const {
    return this->version;
}

void IPH::Domain::Project::setDescription(const std::string &description) {
    this->description = description;
}

std::string IPH::Domain::Project::getDescription() const {
    return this->description;
}

void IPH::Domain::Project::setAnalysis(const std::string &analysis) {
    this->analysis = analysis;
}

std::string IPH::Domain::Project::getAnalysis() const {
    return this->analysis;
}

void IPH::Domain::Project::setSelectedGridDataId(const int &selectedGridDataId) {
    this->selectedGridDataId = selectedGridDataId;
}

long IPH::Domain::Project::getSelectedGridDataId() const {
    return selectedGridDataId;
}

void IPH::Domain::Project::setSelectedHydrodynamicDataId(const long &selectedHydrodynamicDataId) {
    this->selectedHydrodynamicDataId = selectedHydrodynamicDataId;
}

long IPH::Domain::Project::getSelectedHydrodynamicDataId() const {
    return this->selectedHydrodynamicDataId;
}

void IPH::Domain::Project::setSelectedWaterQualityDataId(const long &selectedWaterQualityDataId) {
    this->selectedWaterQualityDataId = selectedWaterQualityDataId;
}

long IPH::Domain::Project::getSelectedWaterQualityDataId() const {
    return selectedWaterQualityDataId;
}

void IPH::Domain::Project::setSelectedSedimentDataId(const long &selectedSedimentDataId) {
    this->selectedSedimentDataId = selectedSedimentDataId;
}

long IPH::Domain::Project::getSelectedSedimentDataId() const {
    return selectedSedimentDataId;
}

void IPH::Domain::Project::setSelectedMeteorologicalDataId(const long &selectedMeteorologicalDataId) {
    this->selectedMeteorologicalDataId = selectedMeteorologicalDataId;
}

long IPH::Domain::Project::getSelectedMeteorologicalDataId() const {
    return this->selectedMeteorologicalDataId;
}
