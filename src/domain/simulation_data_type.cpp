#include <domain/simulation_data_type.h>
#include <cstring>

SimulationDataType::StructuredMesh::StructuredMesh() :
    xCoordinates(nullptr),
    yCoordinates(nullptr),
    northNeighbors(nullptr),
    westNeighbors(nullptr),
    southNeighbors(nullptr),
    eastNeighbors(nullptr),
    verticeIds(nullptr)
{}

void SimulationDataType::StructuredMesh::destroy() {
	delete xCoordinates;
    delete yCoordinates;
    delete northNeighbors;
    delete westNeighbors;
    delete southNeighbors;
    delete eastNeighbors;
    delete verticeIds;
}

SimulationDataType::UnstructuredMesh::UnstructuredMesh() :
    xCoordinates(nullptr),
    yCoordinates(nullptr),
    verticeIds(nullptr)
{}

void SimulationDataType::UnstructuredMesh::destroy() {
	delete xCoordinates;
    delete yCoordinates;
    delete verticeIds;
}

SimulationDataType::GridData::GridData() :
    weights(nullptr)
{}

void SimulationDataType::GridData::destroy() {
    delete weights;
}

SimulationDataType::GridDataConfiguration::GridDataConfiguration() :
    layers(nullptr),
    structuredMesh(nullptr),
    unstructuredMesh(nullptr)
{}

void SimulationDataType::GridDataConfiguration::destroy() {
    if (layers) {
        layers->destroy();
        delete layers;
    }

    if (structuredMesh) {
        structuredMesh->destroy();
        delete structuredMesh;
    }
    
    if (unstructuredMesh) {
        unstructuredMesh->destroy();
        delete unstructuredMesh;
    }
}

SimulationDataType::HydrodynamicParameter::HydrodynamicParameter() {
    memset(name, ' ', sizeof(name));
}

SimulationDataType::NonVerticallyIntegratedRange::NonVerticallyIntegratedRange() :
    timeSeriesList(nullptr)
{}

void SimulationDataType::NonVerticallyIntegratedRange::destroy() {
    delete timeSeriesList;
}

SimulationDataType::BoundaryCondition::BoundaryCondition() :
    ranges(nullptr),
    cells(nullptr),
    timeSeriesList(nullptr)
{
    memset(conditionType, ' ', sizeof(conditionType));
}

void SimulationDataType::BoundaryCondition::destroy() {
    delete ranges;
	delete cells;
    delete timeSeriesList;
}

SimulationDataType::HydrodynamicConfiguration::HydrodynamicConfiguration() :
    parameters(nullptr),
    boundaryConditions(nullptr),
    gridDataConfiguration(nullptr)
{}

void SimulationDataType::HydrodynamicConfiguration::destroy() {
    delete parameters;
    
    if (gridDataConfiguration) {
        gridDataConfiguration->destroy();
        delete gridDataConfiguration;
    }
    
    if (boundaryConditions) {
        boundaryConditions->destroy();
        delete boundaryConditions;
    }
}

SimulationDataType::WaterQualityGroup::WaterQualityGroup() :
    values(nullptr)
{
    memset(name, ' ', sizeof(name));
}

void SimulationDataType::WaterQualityGroup::destroy() {
    delete values;
}

SimulationDataType::WaterQualityParameter::WaterQualityParameter() :
    groups(nullptr)
{
    memset(name, ' ', sizeof(name));
}

void SimulationDataType::WaterQualityParameter::destroy() {
    delete groups;
}

SimulationDataType::FoodMatrixValue::FoodMatrixValue() {
    memset(predator, ' ', sizeof(predator));
    memset(prey, ' ', sizeof(prey));
}

SimulationDataType::WaterQualityConfiguration::WaterQualityConfiguration() :
    parameters(nullptr),
    foodMatrix(nullptr),
    boundaryConditions(nullptr)
{}

void SimulationDataType::WaterQualityConfiguration::destroy() {
    if (parameters) {
        parameters->destroy();
        delete parameters;
    }
    
    delete foodMatrix;
    
    if (boundaryConditions) {
        boundaryConditions->destroy();
        delete boundaryConditions;
    }
}

SimulationDataType::MeteorologicalParameter::MeteorologicalParameter() :
    name(nullptr),
    timeSeriesList(nullptr)
{}

void SimulationDataType::MeteorologicalParameter::destroy() {
	delete name;
    delete timeSeriesList;
}

SimulationDataType::MeteorologicalStation::MeteorologicalStation() :
    parameters(nullptr)
{}

void SimulationDataType::MeteorologicalStation::destroy() {
    if (parameters) {
        parameters->destroy();
        delete parameters;
    }
}

SimulationDataType::MeteorologicalConfiguration::MeteorologicalConfiguration() :
    stations(nullptr)
{}

void SimulationDataType::MeteorologicalConfiguration::destroy() {
    if (stations) {
        stations->destroy();
        delete stations;
    }
}

SimulationDataType::OutputParameter::OutputParameter() {
    memset(name, ' ', sizeof(name));
}

SimulationDataType::RecoveryVariables::RecoveryVariables() :
    u(nullptr),
    w(nullptr),
    eta(nullptr),
    wqo(nullptr)
{}

void SimulationDataType::RecoveryVariables::destroy() {
	delete u;
    delete w;
    delete eta;
}

SimulationDataType::Simulation::Simulation() :
    label(nullptr),
    layers(nullptr),
    hydrodynamicConfiguration(nullptr),
    waterQualityConfiguration(nullptr),
    meteorologicalConfiguration(nullptr),
    observations(nullptr),
    outputDirectory(nullptr),
    hydroOutputParameters(nullptr),
    wqOutputParameters(nullptr),
    wqoParameters(nullptr),
    recoveryVariables(nullptr)
{}

void SimulationDataType::Simulation::destroy() {
    if (hydrodynamicConfiguration) {
        hydrodynamicConfiguration->destroy();
        delete hydrodynamicConfiguration;
    }
    
    if (waterQualityConfiguration) {
        waterQualityConfiguration->destroy();
        delete waterQualityConfiguration;
    }
    
    if (meteorologicalConfiguration) {
        meteorologicalConfiguration->destroy();
        delete meteorologicalConfiguration;
    }
    
    delete label;
    delete layers;
    delete observations;
    delete hydroOutputParameters;
    delete wqOutputParameters;
    delete wqoParameters;
    delete outputDirectory;
    
    // Intel Fortran deallocates automatically
    #if !defined(_WIN32) && !defined(_WIN64)
    if (recoveryVariables) {
        recoveryVariables->destroy();
        delete recoveryVariables;
    }
    #endif
}
