subroutine startSimulation(configuration) bind(C, name="startSimulation")
	!DEC$ ATTRIBUTES DLLEXPORT :: startSimulation
	use domain_types
	
	type(HydrodynamicConfiguration) :: configuration
	type(BoundaryCondition), pointer :: boundaryConditions(:)

	call c_f_pointer(configuration%boundaryConditions, boundaryConditions, [configuration%numberOfBoundaryConditions])

	print *, boundaryConditions(1)%constantValue
end subroutine