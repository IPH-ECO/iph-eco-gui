#ifndef SIMULATION_DATA_TYPE_H
#define SIMULATION_DATA_TYPE_H

namespace SimulationDataType {
    extern "C" {
        struct StructuredMesh {
            long long int numberOfElements;
			unsigned int resolution;
            double *xCoordinates;
            double *yCoordinates;
			long long int *northNeighbors;
			long long int *westNeighbors;
			long long int *southNeighbors;
			long long int *eastNeighbors;
        };
        
        struct UnstructuredMesh {
			long long int numberOfPoints;
            double *xCoordinates;
            double *yCoordinates;
			long long int numberOfElements;
			long long int *verticeIds;
        };
        
        struct GridData {
			long long int numberOfElements;
            double *weights;
            int type;
        };
        
        struct GridDataConfiguration {
            int numberOfLayers;
            SimulationDataType::GridData *layers;
            bool isStructured;
            SimulationDataType::StructuredMesh *structuredMesh;
            SimulationDataType::UnstructuredMesh *unstructuredMesh;
        };
        
        struct HydrodynamicParameter {
            char *name;
            int length;
            double value;
        };
        
        struct TimeSeries {
            int timeStamp;
            double value;
        };
        
        struct BoundaryCondition {
            int conditionType;
            int numberOfObjects;
			long long int *objectIds;
            int conditionFunction;
            double constantValue;
            int timeSeriesListSize;
            SimulationDataType::TimeSeries *timeSeriesList;
            bool verticalIntegratedOutflow;
            double quota;
        };
        
        struct HydrodynamicConfiguration {
            int numberOfParameters;
            SimulationDataType::HydrodynamicParameter *parameters;
            int numberOfBoundaryConditions;
            SimulationDataType::BoundaryCondition *boundaryConditions;
            SimulationDataType::GridDataConfiguration *gridDataConfiguration;
        };

        struct Simulation {
            int modules;
            int labelLength;
            char *label;
            int simulationType;
            unsigned int initialTime;
            double period;
            int stepTime;
            int layersLength;
            double *layers;
            HydrodynamicConfiguration *hydrodynamicConfiguration;
            int observationsLength;
            char *observations;
        };
    }
}

#endif // SIMULATION_DATA_TYPE_H
