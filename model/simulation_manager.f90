subroutine startSimulation(sim) bind(C, name="startSimulation")
	!DEC$ ATTRIBUTES DLLEXPORT :: startSimulation
	use domain_types
	
    type(Simulation) :: sim
    type(HydrodynamicConfiguration), pointer :: hydroConfiguration
    type(BoundaryCondition), pointer :: boundaryConditions(:)
    type(BoundaryConditionCell), pointer :: cells(:)
	
	call c_f_pointer(sim%hydrodynamicConfiguration, hydroConfiguration)
	call c_f_pointer(hydroConfiguration%boundaryConditions, boundaryConditions, [hydroConfiguration%numberOfBoundaryConditions])
	
    do i = 1, hydroConfiguration%numberOfBoundaryConditions
    	call c_f_pointer(boundaryConditions(i)%cells, cells, [boundaryConditions(i)%cellsLength])

    	do j = 1, boundaryConditions(i)%cellsLength
		    print *, cells(j)%cellId, cells(j)%verticeIds(1), cells(j)%verticeIds(2)
    	end do
    end do
end subroutine
