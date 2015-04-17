#include "include/domain/grid_data_configuration.h"

GridDataConfiguration::GridDataConfiguration() {}

QString GridDataConfiguration::getName() const {
    return this->name;
}

void GridDataConfiguration::setName(const QString &name) {
    this->name = name;
}

QSet<GridData*>& GridDataConfiguration::getGridDataSet() const {
    return &gridDataSet;
}

bool GridDataConfiguration::addGridData(GridData *gridData) {
    if (this->gridDataSet.contains(gridData)) {
        return false;
    }

    this->gridDataSet.insert(gridData);
    return true;
}

void GridDataConfiguration::removeGridData(GridData *gridData) {
    this->gridDataSet.remove(gridData);
    delete gridData;
}

GridData* GridDataConfiguration::getGridData(const GridData::GridInputType &gridInputType, const QString &gridInputFile, const GridData::GridInformationType gridInformationType) {
    for (QSet<GridData*>::const_iterator it = gridDataSet.begin(); it != gridDataSet.end(); it++) {
        GridData *&gridData = (*it);
        if (gridData->getGridInputType() == gridInputType && gridData->getInputFile() == gridInputFile && gridData->getGridInformationType() == gridInformationType) {
            return gridData;
        }
    }

    return NULL;
}
