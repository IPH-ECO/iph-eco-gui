subroutine startSimulation(sim) bind(C, name="startSimulation")
	!DEC$ ATTRIBUTES DLLEXPORT :: startSimulation
	use domain_types
	
    type(Simulation) :: sim
    type(SimulationStatus), pointer :: simStatus

    allocate(simStatus)

    simStatus%statusCode = 198868
    
    sim%simulationStatus = c_loc(simStatus)
end subroutine
