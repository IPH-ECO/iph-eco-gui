#include <domain/project.h>

#include <QSetIterator>

Project::Project(const QString &name, const QString &description, const bool &hydrodynamic, const bool &waterQuality) :
    id(0),
    name(name),
    description(description),
    hydrodynamic(hydrodynamic),
    waterQuality(waterQuality)
{}

Project::~Project() {
    for (Mesh *mesh : meshes) {
        delete mesh;
    }
    
    for (GridDataConfiguration *configuration : gridDataConfigurations) {
        delete configuration;
    }
    
    for (HydrodynamicConfiguration *configuration : hydrodynamicConfigurations) {
        delete configuration;
    }
    
    for (MeteorologicalConfiguration *configuration : meteorologicalConfigurations) {
        delete configuration;
    }
    
    for (Simulation *simulation : simulations) {
        delete simulation;
    }
}

void Project::setId(const uint &id) {
    if (!isPersisted()) {
        this->id = id;
    }
}

uint Project::getId() const {
    return id;
}

bool Project::isPersisted() const {
    return this->id != 0;
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

QSet<Mesh*> Project::getMeshes() const {
    return meshes;
}

bool Project::addMesh(Mesh *mesh) {
    if (meshes.contains(mesh)) {
        return false;
    }
    
    meshes.insert(mesh);
    
    return true;
}

void Project::removeMesh(Mesh *mesh) {
    for (Simulation *simulation : simulations) {
        if (simulation->getMesh() == mesh) {
            simulations.removeOne(simulation);
            delete simulation;
        }
    }
    
    QSetIterator<GridDataConfiguration*> gridDataConfigurationIterator(gridDataConfigurations);
    
    while (gridDataConfigurationIterator.hasNext()) {
        GridDataConfiguration *gridDataConfiguration = gridDataConfigurationIterator.next();
        
        if (gridDataConfiguration->getMesh() == mesh) {
            QSetIterator<HydrodynamicConfiguration*> hydroConfigurationIterator(hydrodynamicConfigurations);
            
            while (hydroConfigurationIterator.hasNext()) {
                HydrodynamicConfiguration *hydrodynamicConfiguration = hydroConfigurationIterator.next();
                
                if (hydrodynamicConfiguration->getGridDataConfiguration() == gridDataConfiguration) {
                    hydrodynamicConfigurations.remove(hydrodynamicConfiguration);
                    delete hydrodynamicConfiguration;
                }
            }
            
            QSetIterator<MeteorologicalConfiguration*> meteorologicalConfigurationIterator(meteorologicalConfigurations);
            
            while (meteorologicalConfigurationIterator.hasNext()) {
                MeteorologicalConfiguration *meteorologicalConfiguration = meteorologicalConfigurationIterator.next();
                
                if (meteorologicalConfiguration->getGridDataConfiguration() == gridDataConfiguration) {
                    meteorologicalConfigurations.remove(meteorologicalConfiguration);
                    delete meteorologicalConfiguration;
                }
            }
            
            gridDataConfigurations.remove(gridDataConfiguration);
            delete gridDataConfiguration;
        }
    }
    
    meshes.remove(mesh);
    delete mesh;
    
}

bool Project::containsMesh(const QString &meshName) {
    return this->getMesh(meshName) != nullptr;
}

Mesh* Project::getMesh(const QString &meshName) const {
    for (Mesh *mesh : meshes) {
        if (mesh->getName() == meshName) {
            return mesh;
        }
    }

    return nullptr;
}

Mesh* Project::getMesh(const uint &id) const {
    for (Mesh *mesh : meshes) {
        if (mesh->getId() == id) {
            return mesh;
        }
    }
    
    return nullptr;
}

bool Project::addGridDataConfiguration(GridDataConfiguration *gridDataConfiguration) {
    if (this->gridDataConfigurations.contains(gridDataConfiguration)) {
        return false;
    }

    this->gridDataConfigurations.insert(gridDataConfiguration);

    return true;
}

void Project::removeGridDataConfiguration(const QString &configurationName) {
    GridDataConfiguration *gridDataConfiguration = getGridDataConfiguration(configurationName);

    if (gridDataConfiguration) {
        gridDataConfigurations.remove(gridDataConfiguration);
        delete gridDataConfiguration;
    }

}

GridDataConfiguration* Project::getGridDataConfiguration(const QString &configurationName) const {
    for (GridDataConfiguration *gridDataConfiguration : gridDataConfigurations) {
        if (gridDataConfiguration->getName() == configurationName) {
            return gridDataConfiguration;
        }
    }

    return nullptr;
}

GridDataConfiguration* Project::getGridDataConfiguration(const uint &id) const {
    for (GridDataConfiguration *gridDataConfiguration : gridDataConfigurations) {
        if (gridDataConfiguration->getId() == id) {
            return gridDataConfiguration;
        }
    }
    
    return nullptr;
}

QSet<GridDataConfiguration*> Project::getGridDataConfigurations() const {
    return gridDataConfigurations;
}

bool Project::addHydrodynamicConfiguration(HydrodynamicConfiguration *hydrodynamicConfiguration) {
    if (this->hydrodynamicConfigurations.contains(hydrodynamicConfiguration)) {
        return false;
    }
    
    this->hydrodynamicConfigurations.insert(hydrodynamicConfiguration);
    
    return true;
}

void Project::removeHydrodynamicConfiguration(const QString &configurationName) {
    HydrodynamicConfiguration *hydrodynamicConfiguration = getHydrodynamicConfiguration(configurationName);
    
    if (hydrodynamicConfiguration) {
        hydrodynamicConfigurations.remove(hydrodynamicConfiguration);
        delete hydrodynamicConfiguration;
    }
}

HydrodynamicConfiguration* Project::getHydrodynamicConfiguration(const QString &configurationName) {
    for (HydrodynamicConfiguration *hydrodynamicConfiguration : hydrodynamicConfigurations) {
        if (hydrodynamicConfiguration->getName() == configurationName) {
            return hydrodynamicConfiguration;
        }
    }
    
    return nullptr;
}

HydrodynamicConfiguration* Project::getHydrodynamicConfiguration(uint id) const {
    for (HydrodynamicConfiguration *hydrodynamicConfiguration : hydrodynamicConfigurations) {
        if (hydrodynamicConfiguration->getId() == id) {
            return hydrodynamicConfiguration;
        }
    }
    
    return nullptr;
}

QSet<HydrodynamicConfiguration*> Project::getHydrodynamicConfigurations() const {
    return hydrodynamicConfigurations;
}

bool Project::addWaterQualityConfiguration(WaterQualityConfiguration *waterQualityConfiguration) {
    if (this->waterQualityConfigurations.contains(waterQualityConfiguration)) {
        return false;
    }
    
    this->waterQualityConfigurations.insert(waterQualityConfiguration);
    
    return true;
}

void Project::removeWaterQualityConfiguration(const QString &configurationName) {
    WaterQualityConfiguration *waterQualityConfiguration = getWaterQualityConfiguration(configurationName);
    
    if (waterQualityConfiguration) {
        waterQualityConfigurations.remove(waterQualityConfiguration);
        delete waterQualityConfiguration;
    }
}

WaterQualityConfiguration* Project::getWaterQualityConfiguration(const QString &configurationName) {
    for (WaterQualityConfiguration *waterQualityConfiguration : waterQualityConfigurations) {
        if (waterQualityConfiguration->getName() == configurationName) {
            return waterQualityConfiguration;
        }
    }
    
    return nullptr;
}

WaterQualityConfiguration* Project::getWaterQualityConfiguration(uint id) const {
    for (WaterQualityConfiguration *waterQualityConfiguration : waterQualityConfigurations) {
        if (waterQualityConfiguration->getId() == id) {
            return waterQualityConfiguration;
        }
    }
    
    return nullptr;
}

QSet<WaterQualityConfiguration*> Project::getWaterQualityConfigurations() const {
    return waterQualityConfigurations;
}

bool Project::addMeteorologicalConfiguration(MeteorologicalConfiguration *meteorologicalConfiguration) {
    if (this->meteorologicalConfigurations.contains(meteorologicalConfiguration)) {
        return false;
    }
    
    this->meteorologicalConfigurations.insert(meteorologicalConfiguration);
    
    return true;
}

void Project::removeMeteorologicalConfiguration(const QString &configurationName) {
    MeteorologicalConfiguration *configuration = this->getMeteorologicalConfiguration(configurationName);
    
    if (configuration) {
        this->meteorologicalConfigurations.remove(configuration);
        delete configuration;
    }
}

MeteorologicalConfiguration* Project::getMeteorologicalConfiguration(const QString &configurationName) const {
    for (MeteorologicalConfiguration *configuration : this->meteorologicalConfigurations) {
        if (configuration->getName() == configurationName) {
            return configuration;
        }
    }

    return nullptr;
}

MeteorologicalConfiguration* Project::getMeteorologicalConfiguration(uint id) const {
    for (MeteorologicalConfiguration *configuration : this->meteorologicalConfigurations) {
        if (configuration->getId() == id) {
            return configuration;
        }
    }
    
    return nullptr;
}

QSet<MeteorologicalConfiguration*> Project::getMeteorologicalConfigurations() const {
    return this->meteorologicalConfigurations;
}

bool Project::addSimulation(Simulation *simulation) {
    if (simulations.contains(simulation)) {
        return false;
    }
    
    simulations.append(simulation);
    
    emit simulationCreated(simulation);
    
    return true;
}

void Project::removeSimulation(const QString &simulationLabel) {
    Simulation *simulation = getSimulation(simulationLabel);
    
    if (simulation) {
        simulations.removeOne(simulation);
        delete simulation;
    }
}

Simulation* Project::getSimulation(const QString &label) const {
    for (Simulation *simulation : simulations) {
        if (simulation->getLabel() == label) {
            return simulation;
        }
    }
    
    return nullptr;
}

QList<Simulation*> Project::getSimulations() const {
    return simulations;
}