subroutine startSimulation(sim) bind(C, name="startSimulation")
	!DEC$ ATTRIBUTES DLLEXPORT :: startSimulation
	use domain_types
	
    type(Simulation) :: sim
    type(WaterQualityConfiguration), pointer :: configuration
    type(WaterQualityParameter), dimension(:), pointer :: parameters
    type(WaterQualityGroup), dimension(:), pointer :: groups
    real(c_double), dimension(:), pointer :: values
    integer(c_int) :: groupQuantity

    call c_f_pointer(sim%waterQualityConfiguration, configuration)
    call c_f_pointer(configuration%parameters, parameters, [configuration%numberOfParameters])

    do i = 1, configuration%numberOfParameters
        write(*, *) parameters(i)%name

        if (parameters(i)%numberOfGroups == 0) then
            ! hasn't group values. we need to get the first one from values array.
            write(*, *) 'INLINE: ', parameters(i)%value
        else
            ! has group values
            call c_f_pointer(parameters(i)%groups, groups, [parameters(i)%numberOfGroups])

            do j = 1, parameters(i)%numberOfGroups
                call c_f_pointer(groups(j)%values, values, [groups(j)%numberOfValues])

                write(*, *) groups(j)%name

                do k = 1, groups(j)%numberOfValues
                    write(*, *) 'VALUE(', k - 1, ')=', values(k)
                end do
            end do
        end if
    end do
end subroutine