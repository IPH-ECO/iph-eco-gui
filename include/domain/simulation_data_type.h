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
            
            void destroy();
        };
        
        struct UnstructuredMesh {
			long long int numberOfPoints;
            double *xCoordinates;
            double *yCoordinates;
			long long int verticeIdsLength;
			long long int *verticeIds;
            
            void destroy();
        };
        
        struct GridData {
			long long int numberOfElements;
            double *weights;
            int typeId;
            
            void destroy();
        };
        
        struct GridDataConfiguration {
            int numberOfLayers;
            SimulationDataType::GridData *layers;
            bool isStructured;
            SimulationDataType::StructuredMesh *structuredMesh;
            SimulationDataType::UnstructuredMesh *unstructuredMesh;
            
            void destroy();
        };
        
        struct HydrodynamicParameter {
            char *name;
            int length;
            double value;
            
            void destroy();
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
            
            void destroy();
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
            
            void destroy();
        };
        
        struct MeteorologicalStation {
            double utmX;
            double utmY;
            int parametersLength;
            SimulationDataType::MeteorologicalParameter *parameters;
            
            void destroy();
        };
        
        struct MeteorologicalConfiguration {
            int stationsLength;
            SimulationDataType::MeteorologicalStation *stations;
            
            void destroy();
        };

        struct HydrodynamicConfiguration {
            int numberOfParameters;
            SimulationDataType::HydrodynamicParameter *parameters;
            int numberOfBoundaryConditions;
            SimulationDataType::BoundaryCondition *boundaryConditions;
            SimulationDataType::GridDataConfiguration *gridDataConfiguration;
            
            void destroy();
        };

        struct OutputParameter {
            int nameLength;
            char *name;
            
            void destroy();
        };
        
        struct RecoveryVariables {
            // Hydrodynamic module
            double *u;
            int layers;
            int edges;
            double *w;
            int elements;
            double *eta;
            int simulationTime;
            bool changed;

            void destroy();
        };

        struct Simulation {
            bool hydrodynamic;
            bool waterQuality;
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
            int outputTimeInterval;
            int autosaveTimeInterval;
            int outputParametersLength;
            SimulationDataType::OutputParameter *outputParameters;
            SimulationDataType::RecoveryVariables *recoveryVariables; // Intel Fortran deallocates automatically
            int statusCode;
            int progress;
            
            void destroy();
        };
    }
}

#endif // SIMULATION_DATA_TYPE_H
