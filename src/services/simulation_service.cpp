#include "include/services/simulation_service.h"

#include <vtkQuad.h>
#include <vtkIdList.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkCellCenters.h>

extern "C" {
    void startSimulation(SimulationDataType::HydrodynamicConfiguration *configuration);
}

SimulationService* SimulationService::instance = nullptr;

SimulationService::SimulationService() {}

SimulationService* SimulationService::getInstance() {
	if (!instance) {
		instance = new SimulationService();
	}

	return instance;
}

void SimulationService::run(HydrodynamicConfiguration *hydrodynamicConfiguration) {
    SimulationDataType::HydrodynamicConfiguration sHydroConfiguration = hydrodynamicConfiguration->toSimulationDataType();
    
    startSimulation(&sHydroConfiguration);
}
