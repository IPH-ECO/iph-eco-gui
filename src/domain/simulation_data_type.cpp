#include <domain/simulation_data_type.h>

void SimulationDataType::StructuredMesh::destroy() {
	delete xCoordinates;
    delete yCoordinates;
    delete northNeighbors;
    delete westNeighbors;
    delete southNeighbors;
    delete eastNeighbors;
    delete verticeIds;
}

void SimulationDataType::UnstructuredMesh::destroy() {
	delete xCoordinates;
    delete yCoordinates;
    delete verticeIds;
}

void SimulationDataType::GridData::destroy() {
    delete weights;
}

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

void SimulationDataType::HydrodynamicParameter::destroy() {
    delete name;
}

void SimulationDataType::BoundaryCondition::destroy() {
	delete cells;
    delete timeSeriesList;
}

void SimulationDataType::MeteorologicalParameter::destroy() {
	delete name;
    delete timeSeriesList;
}

void SimulationDataType::MeteorologicalStation::destroy() {
	parameters->destroy();
    delete parameters;
}

void SimulationDataType::MeteorologicalConfiguration::destroy() {
	stations->destroy();
    delete stations;
}

void SimulationDataType::HydrodynamicConfiguration::destroy() {
	parameters->destroy();
    boundaryConditions->destroy();
    gridDataConfiguration->destroy();
    
    delete parameters;
    delete boundaryConditions;
    delete gridDataConfiguration;
}

void SimulationDataType::OutputParameter::destroy() {
    delete name;
}

void SimulationDataType::RecoveryVariables::destroy() {
	delete u;
    delete w;
    delete eta;
}

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

    #if !defined(_WIN32) && !defined(_WIN64)
    recoveryVariables->destroy();
    delete recoveryVariables;
    #endif
}