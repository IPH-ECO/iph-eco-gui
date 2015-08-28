subroutine startSimulation(sim) bind(C, name="startSimulation")
	!DEC$ ATTRIBUTES DLLEXPORT :: startSimulation
	use domain_types
	
    type(Simulation) :: sim
    character, pointer :: simulationLabel(:)
    type(HydrodynamicConfiguration), pointer :: configuration
	!type(BoundaryCondition), pointer :: boundaryConditions(:)

	call c_f_pointer(sim%label, simulationLabel, [sim%labelLength])
	call c_f_pointer(sim%hydrodynamicConfiguration, configuration)
    
    print *, simulationLabel
    print *, configuration%numberOfParameters
end subroutine
