subroutine startSimulation(sim) bind(C, name="startSimulation")
	!DEC$ ATTRIBUTES DLLEXPORT :: startSimulation
	use domain_types
	
    type(Simulation) :: sim
    type(SimulationStatus), pointer :: simStatus
    type(OutputParameter), pointer :: parameters(:)
    character, pointer :: parameterName(:)

    allocate(simStatus)

    simStatus%statusCode = 198868
    sim%simulationStatus = c_loc(simStatus)

	call c_f_pointer(sim%outputParameters, parameters, [sim%outputParametersLength])

    do i = 1, sim%outputParametersLength
		call c_f_pointer(parameters(i)%name, parameterName, [parameters(i)%nameLength])
    	print *, parameterName
    end do

end subroutine
