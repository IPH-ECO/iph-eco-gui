subroutine startSimulation(sim) bind(C, name="startSimulation")
	!DEC$ ATTRIBUTES DLLEXPORT :: startSimulation
	use domain_types
	
    type(Simulation) :: sim
	!type(HydrodynamicConfiguration) :: configuration
	!type(BoundaryCondition), pointer :: boundaryConditions(:)

	!call c_f_pointer(configuration%boundaryConditions, boundaryConditions, [configuration%numberOfBoundaryConditions])

	!print *, boundaryConditions(1)%constantValue

    print *, sim%hydrodynamicConfiguration%numberOfParameters
end subroutine
