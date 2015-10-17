module domain_types
    use iso_c_binding

    type, bind(C) :: StructuredMesh
        integer(c_long_long) :: numberOfElements
        real(c_double) :: resolution
        type(c_ptr) :: xCoordinates
        type(c_ptr) :: yCoordinates
        type(c_ptr) :: northNeighbors
        type(c_ptr) :: westNeighbors
        type(c_ptr) :: southNeighbors
        type(c_ptr) :: eastNeighbors
        integer(c_long_long) :: verticeIdsLength
        type(c_ptr) :: verticeIds
    end type

    type, bind(C) :: UnstructuredMesh
        integer(c_long_long) :: numberOfPoints
        type(c_ptr) :: xCoordinates
        type(c_ptr) :: yCoordinates
        integer(c_long_long) :: verticeIdsLength
        type(c_ptr) :: verticeIds
    end type

    type, bind(C) :: GridData
        integer(c_long_long) :: numberOfElements
        type(c_ptr) :: weights
        integer(c_int) :: typeId
    end type

    type, bind(C) :: GridDataConfiguration
        integer(c_int) :: numberOfLayers
        type(c_ptr) :: layers
        logical(c_bool) :: isStructured
        type(c_ptr) :: structuredMesh
        type(c_ptr) :: unstructuredMesh
    end type

    type, bind(C) :: HydrodynamicParameter
        type(c_ptr) :: name
        integer(c_int) :: length
        real(c_double) :: value
    end type

    type, bind(C) :: BoundaryConditionCell
        integer(c_long_long) :: cellId;
        integer(c_long_long) :: verticeIds(2);
    end type

    type, bind(C) :: BoundaryCondition
        integer(c_int) :: conditionType
        integer(c_int) :: cellsLength
        type(c_ptr) :: cells
        integer(c_int) :: conditionFunction
        real(c_double) :: constantValue
        integer(c_int) :: timeSeriesListSize
        type(c_ptr) :: timeSeriesList
        logical(c_bool) :: verticalIntegratedOutflow
        real(c_double) :: quota
    end type

    type, bind(C) :: TimeSeries
        integer(c_int) :: timeStamp
        real(c_double) :: value1
        real(c_double) :: value2
    end type

    type, bind(C) :: MeteorologicalParameter
        integer(c_int) :: nameLength
        type(c_ptr) :: name
        integer(c_int) :: functionMet
        real(c_double) :: constantValue
        logical(c_bool) :: useXYComponent
        real(c_double) :: xComponent
        real(c_double) :: yComponent
        real(c_double) :: intensity
        real(c_double) :: direction
        integer(c_int) :: timeSizeListLength
        type(c_ptr) :: timeSeriesList
    end type

    type, bind(C) :: MeteorologicalStation
        real(c_double) :: utmX
        real(c_double) :: utmY
        integer(c_int) :: parametersLength
        type(c_ptr) :: parameters
    end type

    type, bind(C) :: MeteorologicalConfiguration
        integer(c_int) :: stationsLength
        type(c_ptr) :: stations
    end type

    type, bind(C) :: HydrodynamicConfiguration
        integer(c_int) :: numberOfParameters
        type(c_ptr) :: parameters
        integer(c_int) :: numberOfBoundaryConditions
        type(c_ptr) :: boundaryConditions
        type(c_ptr) :: gridDataConfiguration
    end type

    type, bind(C) :: OutputParameter
        integer(c_int) :: nameLength
        type(c_ptr) :: name
    end type

    type, bind(C) :: SimulationStatus
        integer(c_int) :: statusCode ! RUNNING = 1, PAUSED = 2, STOPPED = 3
    end type

    type, bind(C) :: Simulation
        logical(c_bool) :: hydrodynamic
        logical(c_bool) :: waterQuality
        logical(c_bool) :: sediment
        integer(c_int) :: labelLength
        type(c_ptr) :: label
        integer(c_int) :: simulationType
        integer(c_int) :: initialTime
        real(c_double) :: period
        integer(c_int) :: stepTime
        integer(c_int) :: layersLength
        type(c_ptr) :: layers
        type(c_ptr) :: hydrodynamicConfiguration
        type(c_ptr) :: meteorologicalConfiguration
        real(c_double) :: minimumVerticalLimit
        real(c_double) :: maximumVerticalLimit
        integer(c_int) :: observationsLength
        type(c_ptr) :: observations
        integer(c_int) :: outputDirectoryLength
        type(c_ptr) :: outputDirectory
        integer(c_int) :: outputTimeInterval
        integer(c_int) :: autosaveTimeInterval
        integer(c_int) :: outputParametersLength
        type(c_ptr) :: outputParameters
        integer(c_int) :: statusCode
        integer(c_int) :: progress
    end type

    type, bind(C) :: AutosaveVariables
        type(c_ptr) :: u
        integer(c_int) :: layers
        integer(c_int) :: edges
        type(c_ptr) :: w
        integer(c_int) :: elements
        type(c_ptr) :: eta
        integer(c_int) :: time
    end type
end module
