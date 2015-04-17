#include "include/domain/grid_data.h"

GridData::GridData() : mesh(NULL) {}

GridData::GridData(Mesh *mesh) : mesh(mesh) {}

GridData::GridInputType GridData::getGridInputType() const {
    return gridInputType;
}

void GridData::setGridInputType(const GridData::GridInputType gridInputType) {
    this->gridInputType = gridInputType;
}

QString GridData::getInputFile() const {
    return inputFile;
}

void GridData::setInputFile(const QString &inputFile) {
    this->inputFile = inputFile;
}

GridData::GridInformationType GridData::getGridInformationType() const {
    return gridInformationType;
}

void GridData::setGridInformationType(const GridData::GridInformationType &gridInformationType) {
    this->gridInformationType = gridInformationType;
}

double GridData::getExponent() const {
    return exponent;
}

void GridData::setExponent(const double &exponent) {
    this->exponent = exponent;
}

double GridData::getRadius() const {
    return radius;
}

void GridData::setRadius(const double &radius) {
    this->radius = radius;
}

Mesh* GridData::getMesh() const {
    return mesh;
}

void GridData::setMesh(Mesh *mesh) {
    this->mesh = mesh;
}

QString GridData::gridInputTypeToString() const {
    switch (gridInputType) {
    case POINT:
        return "Point";
    case POLYGON:
        return "Polygon";
    default:
        return "";
    }
}

QString GridData::gridInformationTypeToString() const {
    switch (gridInformationType) {
    case BATHYMETRY:
        return "Bathymetry";
    case WIND_REDUCTION:
        return "Wind Reduction Coefficient";
    case CHEZY:
        return "Chezy Coefficient";
    case WETLAND_AREA:
        return "Wetland Area";
    default:
        return "";
    }
}

GridData::GridInformationType GridData::toGridInformationType(const QString gridInformationTypeStr) {
    if (gridInformationTypeStr == "Bathymetry") {
        return BATHYMETRY;
    }
    if (gridInformationTypeStr == "Wind Reduction Coefficient") {
        return WIND_REDUCTION;
    }
    if (gridInformationTypeStr == "Chezy Coefficient") {
        return CHEZY;
    }
    if (gridInformationTypeStr == "Wetland Area") {
        return WETLAND_AREA;
    }
}
