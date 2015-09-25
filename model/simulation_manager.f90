subroutine startSimulation(sim) bind(C, name="startSimulation")
	!DEC$ ATTRIBUTES DLLEXPORT :: startSimulation
	use domain_types
	
    type(Simulation) :: sim
    type(MeteorologicalConfiguration), pointer :: mc
    type(MeteorologicalStation), pointer :: stations(:)
    type(MeteorologicalParameter), pointer :: parameters(:)
	
	call c_f_pointer(sim%meteorologicalConfiguration, mc)
	call c_f_pointer(mc%stations, stations, [mc%stationsLength])
	
    do i = 1, mc%stationsLength
        call c_f_pointer(stations(i)%parameters, parameters, [stations(i)%parametersLength])

        do j = 1, stations(i)%parametersLength
            print *, parameters(j)%constantValue
        end do
    end do
end subroutine
