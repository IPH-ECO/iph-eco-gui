subroutine startSimulation(configuration) bind(C, name="startSimulation")
	!DEC$ ATTRIBUTES DLLEXPORT :: startSimulation
	use domain_types
	
	type(HydrodynamicConfiguration) :: configuration
	
	print *, configuration%numberOfParameters
end subroutine