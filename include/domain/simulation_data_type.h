#ifndef SIMULATION_DATA_TYPE_H
#define SIMULATION_DATA_TYPE_H

namespace SimulationDataType {
    extern "C" {
        struct StructuredMesh {
            long long int numberOfElements;
			double resolution;
            double *xCoordinates;
            double *yCoordinates;
			long long int *northNeighbors;
			long long int *westNeighbors;
			long long int *southNeighbors;
			long long int *eastNeighbors;
            long long int verticeIdsLength;
            long long int *verticeIds;
        };
        
        struct UnstructuredMesh {
			long long int numberOfPoints;
            double *xCoordinates;
            double *yCoordinates;
			long long int verticeIdsLength;
			long long int *verticeIds;
        };
        
        struct GridData {
			long long int numberOfElements;
            double *weights;
            int typeId;
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
            double value1;
            double value2;
        };
        
        struct BoundaryConditionCell {
            long long int cellId;
            long long int verticeIds[2];
        };
        
        struct BoundaryCondition {
            int conditionType;
            int cellsLength;
            SimulationDataType::BoundaryConditionCell *cells;
            int conditionFunction;
            double constantValue;
            int timeSeriesListSize;
            SimulationDataType::TimeSeries *timeSeriesList;
            bool verticalIntegratedOutflow;
            double quota;
        };
        
        struct MeteorologicalParameter {
            int nameLength;
            char *name;
            int functionMet;
            double constantValue;
            bool useXYComponent;
            double xComponent;
            double yComponent;
            double intensity;
            double direction;
            int timeSizeListLength;
            SimulationDataType::TimeSeries *timeSeriesList;
        };
        
        struct MeteorologicalStation {
            double utmX;
            double utmY;
            int parametersLength;
            SimulationDataType::MeteorologicalParameter *parameters;
        };
        
        struct MeteorologicalConfiguration {
            int stationsLength;
            SimulationDataType::MeteorologicalStation *stations;
        };

        struct HydrodynamicConfiguration {
            int numberOfParameters;
            SimulationDataType::HydrodynamicParameter *parameters;
            int numberOfBoundaryConditions;
            SimulationDataType::BoundaryCondition *boundaryConditions;
            SimulationDataType::GridDataConfiguration *gridDataConfiguration;
        };

        struct OutputParameter {
            int nameLength;
            char *name;
        };

//        struct SimulationStatus {
//            int statusCode;
//        };

        struct Simulation {
            bool hydrodynamic;
            bool waterQuality;
            bool sediment;
            int labelLength;
            char *label;
            int simulationType;
            unsigned int initialTime;
            double period;
            int stepTime;
            int layersLength;
            double *layers;
            SimulationDataType::HydrodynamicConfiguration *hydrodynamicConfiguration;
            SimulationDataType::MeteorologicalConfiguration *meteorologicalConfiguration;
            double minimumVerticalLimit;
            double maximumVerticalLimit;
            int observationsLength;
            char *observations;
            int outputDirectoryLength;
            char *outputDirectory;
            int outputParametersLength;
            SimulationDataType::OutputParameter *outputParameters;
            int statusCode;
            int progress;
        };
    }
}

#endif // SIMULATION_DATA_TYPE_H
