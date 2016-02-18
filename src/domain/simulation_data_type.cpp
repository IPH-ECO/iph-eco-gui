#include <domain/simulation_data_type.h>

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
	layers->destroy();
    delete layers;

    if (structuredMesh) {
        structuredMesh->destroy();
        delete structuredMesh;
    }
    
    if (unstructuredMesh) {
        unstructuredMesh->destroy();
        delete unstructuredMesh;
    }
}

SimulationDataType::HydrodynamicParameter::HydrodynamicParameter() :
    name(nullptr)
{}

void SimulationDataType::HydrodynamicParameter::destroy() {
    delete name;
}

SimulationDataType::BoundaryCondition::BoundaryCondition() :
    cells(nullptr),
    timeSeriesList(nullptr)
{}

void SimulationDataType::BoundaryCondition::destroy() {
	delete cells;
    delete timeSeriesList;
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
	parameters->destroy();
    delete parameters;
}

SimulationDataType::MeteorologicalConfiguration::MeteorologicalConfiguration() :
    stations(nullptr)
{}

void SimulationDataType::MeteorologicalConfiguration::destroy() {
	stations->destroy();
    delete stations;
}

SimulationDataType::HydrodynamicConfiguration::HydrodynamicConfiguration() :
    parameters(nullptr),
    boundaryConditions(nullptr),
    gridDataConfiguration(nullptr)
{}

void SimulationDataType::HydrodynamicConfiguration::destroy() {
	parameters->destroy();
    boundaryConditions->destroy();
    gridDataConfiguration->destroy();
    
    delete parameters;
    delete boundaryConditions;
    delete gridDataConfiguration;
}

SimulationDataType::OutputParameter::OutputParameter() :
    name(nullptr)
{}

void SimulationDataType::OutputParameter::destroy() {
    delete name;
}

SimulationDataType::RecoveryVariables::RecoveryVariables() :
    u(nullptr),
    w(nullptr),
    eta(nullptr)
{}

void SimulationDataType::RecoveryVariables::destroy() {
	delete u;
    delete w;
    delete eta;
}

SimulationDataType::Simulation::Simulation() :
    label(nullptr),
    layers(nullptr),
    hydrodynamic(nullptr),
    meteorologicalConfiguration(nullptr),
    observations(nullptr),
    outputDirectory(nullptr),
    outputParameters(nullptr),
    recoveryVariables(nullptr)
{}

void SimulationDataType::Simulation::destroy() {
	hydrodynamicConfiguration->destroy();
    meteorologicalConfiguration->destroy();
    outputParameters->destroy();
    
    delete label;
    delete layers;
    delete hydrodynamicConfiguration;
    delete meteorologicalConfiguration;
    delete observations;
    delete outputDirectory;
    delete outputParameters;

    // Intel Fortran deallocates automatically
    #if !defined(_WIN32) && !defined(_WIN64)
    recoveryVariables->destroy();
    delete recoveryVariables;
    #endif
}