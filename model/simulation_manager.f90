subroutine startSimulation(sim) bind(C, name="startSimulation")
	!DEC$ ATTRIBUTES DLLEXPORT :: startSimulation
	use domain_types
	
    type(Simulation) :: sim
    type(HydrodynamicConfiguration), pointer :: hydroConfiguration
    type(GridDataConfiguration), pointer :: gridConfiguration
    type(StructuredMesh), pointer :: sm1
	
	call c_f_pointer(sim%hydrodynamicConfiguration, hydroConfiguration)
	call c_f_pointer(hydroConfiguration%gridDataConfiguration, gridConfiguration)
	call c_f_pointer(gridConfiguration%structuredMesh, sm1)
    
    print *, sm1%verticeIdsLength
    print *, gridConfiguration%numberOfLayers
    print *, hydroConfiguration%numberOfParameters
end subroutine
