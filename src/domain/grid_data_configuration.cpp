#include <domain/grid_data_configuration.h>

#include <domain/structured_mesh.h>
#include <domain/unstructured_mesh.h>

#include <vtkDoubleArray.h>
#include <vtkCellData.h>

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

bool GridDataConfiguration::isPersisted() const {
    return id != 0;
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

QList<GridData*> GridDataConfiguration::getGridData(const GridDataType &gridDataType) const {
    QList<GridData*> gridDataList;
    
    for (GridData *gridData : gridDataVector) {
        if (gridData->getGridDataType() == gridDataType) {
            gridDataList.append(gridData);
        }
    }
    
    return gridDataList;
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
    
    return gridDataVector.first()->getMesh();
}

SimulationDataType::GridDataConfiguration* GridDataConfiguration::toSimulationDataType(const HydrodynamicConfiguration *hydrodynamicConfiguration) const {
    SimulationDataType::GridDataConfiguration *gridDataConfiguration = new SimulationDataType::GridDataConfiguration();
    int i = 0;
    
    gridDataConfiguration->numberOfLayers = gridDataVector.size();
    gridDataConfiguration->layers = new SimulationDataType::GridData[gridDataConfiguration->numberOfLayers];
    
    for (GridData *gridData : gridDataVector) {
        gridDataConfiguration->layers[i++] = gridData->toSimulationDataType();
    }
    
    gridDataConfiguration->isStructured = this->getMesh()->instanceOf("StructuredMesh");
    
    if (gridDataConfiguration->isStructured) {
        gridDataConfiguration->structuredMesh = static_cast<StructuredMesh*>(this->getMesh())->toSimulationDataType(hydrodynamicConfiguration);
    } else {
        gridDataConfiguration->unstructuredMesh = static_cast<UnstructuredMesh*>(this->getMesh())->toSimulationDataType();
    }
    
    return gridDataConfiguration;
}