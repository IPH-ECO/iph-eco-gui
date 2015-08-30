module domain_types
    use iso_c_binding

    type, bind(C) :: StructuredMesh
        integer(c_long_long) :: numberOfElements
        integer(c_int) :: resolution
        type(c_ptr) :: xCoordinates
        type(c_ptr) :: yCoordinates
        type(c_ptr) :: northNeighbors
        type(c_ptr) :: westNeighbors
        type(c_ptr) :: southNeighbors
        type(c_ptr) :: eastNeighbors
    end type

    type, bind(C) :: UnstructuredMesh
        integer(c_long_long) :: numberOfPoints
        type(c_ptr) :: xCoordinates
        type(c_ptr) :: yCoordinates
        integer(c_long_long) :: numberOfElements
        type(c_ptr) :: verticeIds
    end type

    type, bind(C) :: GridData
        integer(c_long_long) :: numberOfElements
        type(c_ptr) :: weights
        integer(c_int) :: type
    end type

    type, bind(C) :: GridDataConfiguration
        integer(c_int) :: numberOfLayers
        type(c_ptr) :: layers
        logical(c_bool) :: isStructured
        type(StructuredMesh) :: structuredMesh
        type(UnstructuredMesh) :: unstructuredMesh
    end type

    type, bind(C) :: HydrodynamicParameter
        type(c_ptr) :: name
        integer(c_int) :: length
        real(c_double) :: value
    end type

    type, bind(C) :: BoundaryCondition
        integer(c_int) :: conditionType
        integer(c_int) :: numberOfObjects
        type(c_ptr) :: objectIds
        integer(c_int) :: conditionFunction
        real(c_double) :: constantValue
        integer(c_int) :: timeSeriesListSize
        type(c_ptr) :: timeSeriesList
        logical(c_bool) :: verticalIntegratedOutflow
        real(c_double) :: quota
    end type

    type, bind(C) :: TimeSeries
        integer(c_int) :: timeStampSize
        type(c_ptr) :: timeStamp
        real(c_double) :: value
    end type

    type, bind(C) :: HydrodynamicConfiguration
        integer(c_int) :: numberOfParameters
        type(c_ptr) :: parameters
        integer(c_int) :: numberOfBoundaryConditions
        type(c_ptr) :: boundaryConditions
        type(GridDataConfiguration) :: gridDataConfiguration
    end type

    ! Hydrodynamic, Water Quality, Sediment = Modules combination code
    ! true, false, false = 1
    ! false, true, false = 2
    ! true, true, false = 3
    ! false, false, true = 4
    ! true, false, true = 5
    ! false, true, true = 6
    ! true, true, true = 7
    type, bind(C) :: Simulation
        integer(c_int) :: modules
        integer(c_int) :: labelLength
        type(c_ptr) :: label
        integer(c_int) :: simulationType
        integer(c_int) :: initialTime
        real(c_double) :: period
        integer(c_int) :: stepTime
        integer(c_int) :: layersLength
        type(c_ptr) :: layers
        type(c_ptr) :: hydrodynamicConfiguration
        integer(c_int) :: observationsLength
        type(c_ptr) :: observations
    end type
end module
