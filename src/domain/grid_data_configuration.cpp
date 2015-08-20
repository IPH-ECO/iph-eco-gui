#include "include/domain/grid_data_configuration.h"

GridDataConfiguration::GridDataConfiguration() : id(0) {}

GridDataConfiguration::~GridDataConfiguration() {
    this->clearGridDataVector();
}

uint GridDataConfiguration::getId() const {
    return id;
}

void GridDataConfiguration::setId(const uint &id) {
    if (!isPersisted()) {
        this->id = id;
    }
}

QString GridDataConfiguration::getName() const {
    return name;
}

void GridDataConfiguration::setName(const QString &name) {
    this->name = name;
}

QVector<GridData*> GridDataConfiguration::getGridDataVector() {
    return gridDataVector;
}

bool GridDataConfiguration::addGridData(GridData *gridData) {
    if (this->gridDataVector.contains(gridData)) {
        return false;
    }

    this->gridDataVector.push_back(gridData);

    return true;
}

void GridDataConfiguration::removeGridData(GridData *gridData) {
    gridData->getMesh()->removeArray(gridData->getName());
    gridDataVector.removeOne(gridData);
    delete gridData;
}

void GridDataConfiguration::removeGridData(int i) {
    GridData *gridData = nullptr;

    if (i < gridDataVector.count()) {
        gridData = gridDataVector.at(i);
        gridData->getMesh()->removeArray(gridData->getName());
        gridDataVector.remove(i);
    }

    delete gridData;
}

GridData* GridDataConfiguration::getGridData(const QString &gridDataName) {
    for (int i = 0; i < gridDataVector.size(); i++) {
        if (gridDataVector.at(i)->getName() == gridDataName) {
            return gridDataVector.at(i);
        }
    }
    
    return nullptr;
}

GridData* GridDataConfiguration::getGridData(int i) {
    if (i < gridDataVector.count()) {
        return gridDataVector.at(i);
    }

    return nullptr;
}

GridData* GridDataConfiguration::getBathymetryGridData() const {
    for (GridData *gridData : gridDataVector) {
        if (gridData->getGridDataType() == GridDataType::BATHYMETRY) {
            return gridData;
        }
    }
    
    return nullptr;
}

GridData* GridDataConfiguration::getRoughnessGridData() const {
    for (GridData *gridData : gridDataVector) {
        if (gridData->getGridDataType() == GridDataType::ROUGHNESS) {
            return gridData;
        }
    }
    
    return nullptr;
}

void GridDataConfiguration::clearGridDataVector() {
    for (int i = 0; i < gridDataVector.size(); i++) {
        delete gridDataVector.at(i);
    }
    gridDataVector.clear();
}

Mesh* GridDataConfiguration::getMesh() const {
    if (gridDataVector.isEmpty()) {
        return nullptr;
    }
    
    return gridDataVector.at(0)->getMesh();
}

bool GridDataConfiguration::isPersisted() const {
    return id != 0;
}