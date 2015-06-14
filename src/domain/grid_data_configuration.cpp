#include "include/domain/grid_data_configuration.h"

GridDataConfiguration::GridDataConfiguration() {}

GridDataConfiguration::~GridDataConfiguration() {
    for (int i = 0; i < gridDataVector.size(); i++) {
        delete gridDataVector.at(i);
    }
}

QString GridDataConfiguration::getName() const {
    return this->name;
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
    gridDataVector.removeOne(gridData);
    delete gridData;
}

void GridDataConfiguration::removeGridData(int i) {
    GridData *gridData = NULL;

    if (i < gridDataVector.count()) {
        gridData = gridDataVector.at(i);
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
    
    return NULL;
}

GridData* GridDataConfiguration::getGridData(int i) {
    if (i < gridDataVector.count()) {
        return gridDataVector.at(i);
    }

    return NULL;
}