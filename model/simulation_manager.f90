subroutine startSimulation(sim) bind(C, name="startSimulation")
	!DEC$ ATTRIBUTES DLLEXPORT :: startSimulation
	use domain_types
	
    type(Simulation) :: sim
    type(AutosaveVariables) :: variables
    integer :: nLayer = 2
    integer :: nEdge = 2
    real(c_double), dimension(:, :), pointer :: u ! Vetor de velocidade horizontal

    call c_f_pointer(sim%autosaveVariables, variables)

    allocate(u(nLayer, nEdge))

    u(1, 1) = 10
    u(2, 1) = 20
    u(1, 2) = 30
    u(2, 2) = 40

    variables%layers = nLayer
    variables%edges = nEdge
    variables%u = c_loc(u)

    ! Progress of simulation
    sim%progress = 0
    print *, '0%'
    
    do i = 1, 1000000000
        do while (sim%statusCode == 3)
        end do

        if (sim%statusCode == 5 .or. sim%statusCode == 4) then
            sim%progress = 100
            return
        end if
    end do

    sim%progress = 25
    print *, '25%'
    
    do i = 1, 1000000000
        do while (sim%statusCode == 3)
        end do

        if (sim%statusCode == 5 .or. sim%statusCode == 4) then
            sim%progress = 100
            return
        end if
    end do

    sim%progress = 50
    print *, '50%'
    
    do i = 1, 1000000000
        do while (sim%statusCode == 3)
        end do

        if (sim%statusCode == 5 .or. sim%statusCode == 4) then
            sim%progress = 100
            return
        end if
    end do

    sim%progress = 75
    print *, '75%'
    
    do i = 1, 1000000000
        do while (sim%statusCode == 3)
        end do

        if (sim%statusCode == 5 .or. sim%statusCode == 4) then
            sim%progress = 100
            return
        end if
    end do

    sim%progress = 100
    print *, '100%'

end subroutine