subroutine startSimulation(sim) bind(C, name="startSimulation")
	!DEC$ ATTRIBUTES DLLEXPORT :: startSimulation
	use domain_types
	
    type(Simulation) :: sim
    type(RecoveryVariables), pointer :: variables
    integer :: nLayer = 3
    integer :: nEdge = 2
    integer :: elements = 10
    integer :: uLength
    real(c_double), dimension(:, :), pointer :: u
    real(c_double), dimension(:, :), pointer :: w
    real(c_double), dimension(:), pointer :: eta

    character, pointer :: dir(:)

    call c_f_pointer(sim%outputDirectory, dir, [sim%outputDirectoryLength])

    print *, dir

    uLength = nLayer * nEdge

    allocate(variables)
    allocate(u(nLayer, nEdge))
    allocate(w(nLayer + 1, elements))
    allocate(eta(elements))

    do i = 1, nLayer
        do j = 1, nEdge
            u(i, j) = i * j
        end do
    end do

    variables%layers = nLayer
    variables%edges = nEdge
    variables%elements = elements
    variables%simulationTime = 681988
    variables%u = c_loc(u)
    variables%w = c_loc(w)
    variables%eta = c_loc(eta)

    do k = 1, elements
        eta(k) = k
    end do


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

    variables%changed = logical(.true., kind = c_bool)
    sim%recoveryVariables = c_loc(variables)
    
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