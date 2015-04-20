#include "include/domain/grid_data_configuration.h"

GridDataConfiguration::GridDataConfiguration() {}

GridDataConfiguration::~GridDataConfiguration() {
    for (int i = 0; i < gridDataVector.size(); i++) {
        delete gridDataVector.at(i);
    }

    gridDataVector.clear();
}

QString GridDataConfiguration::getName() const {
    return this->name;
}

void GridDataConfiguration::setName(const QString &name) {
    this->name = name;
}

Mesh* GridDataConfiguration::getMesh() const {
    return mesh;
}

void GridDataConfiguration::setMesh(Mesh *mesh) {
    this->mesh = mesh;
}

QVector<GridData*>& GridDataConfiguration::getGridDataVector() {
    return gridDataVector;
}

bool GridDataConfiguration::addGridData(GridData *gridData) {
    if (this->gridDataVector.contains(gridData)) {
        return false;
    }

    this->gridDataVector.push_back(gridData);
    return true;
}

void GridDataConfiguration::removeGridData(int i) {
    GridData *gridData = NULL;

    if (i < gridDataVector.count()) {
        gridData = gridDataVector.at(i);
        gridDataVector.remove(i);
    }

    delete gridData;
}

GridData* GridDataConfiguration::getGridData(int i) {
    if (i < gridDataVector.count()) {
        return gridDataVector.at(i);
    }

    return NULL;
}
