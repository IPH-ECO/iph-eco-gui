subroutine startSimulation(sim) bind(C, name="startSimulation")
	!DEC$ ATTRIBUTES DLLEXPORT :: startSimulation
	use domain_types
	
    type(Simulation) :: sim
    type(WaterQualityConfiguration), pointer :: wqc

    call c_f_pointer(sim%waterQualityConfiguration, wqc)

    print *, wqc%numberOfParameters
end subroutine