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
            
            StructuredMesh();
            void destroy();
        };
        
        struct UnstructuredMesh {
			long long int numberOfPoints;
            double *xCoordinates;
            double *yCoordinates;
			long long int verticeIdsLength;
			long long int *verticeIds;
            
            UnstructuredMesh();
            void destroy();
        };
        
        struct GridData {
			long long int numberOfElements;
            double *weights;
            int typeId;
            
            GridData();
            void destroy();
        };
        
        struct GridDataConfiguration {
            int numberOfLayers;
            SimulationDataType::GridData *layers;
            bool isStructured;
            SimulationDataType::StructuredMesh *structuredMesh;
            SimulationDataType::UnstructuredMesh *unstructuredMesh;
            
            GridDataConfiguration();
            void destroy();
        };
        
        struct HydrodynamicParameter {
            char *name;
            int length;
            double value;
            
            HydrodynamicParameter();
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
            double minimumElevation;
            double maximumElevation;
            
            BoundaryCondition();
            void destroy();
        };
        
        struct HydrodynamicConfiguration {
            int numberOfParameters;
            SimulationDataType::HydrodynamicParameter *parameters;
            int numberOfBoundaryConditions;
            SimulationDataType::BoundaryCondition *boundaryConditions;
            SimulationDataType::GridDataConfiguration *gridDataConfiguration;
            
            HydrodynamicConfiguration();
            void destroy();
        };
        
        struct WaterQualityGroup {
            int nameLength;
            char *name;
            
            WaterQualityGroup();
            void destroy();
        };
        
        struct WaterQualityParameter {
            int nameLength;
            char *name;
            int parameterType;
            int numberOfGroups;
            WaterQualityGroup *groups;
            int numberOfValues;
            double *values;
            
            WaterQualityParameter();
            void destroy();
        };
        
        struct WaterQualityConfiguration {
            int numberOfParameters;
            SimulationDataType::WaterQualityParameter *parameters;
            
            WaterQualityConfiguration();
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
          
            MeteorologicalParameter();
            void destroy();
        };
        
        struct MeteorologicalStation {
            double utmX;
            double utmY;
            int parametersLength;
            SimulationDataType::MeteorologicalParameter *parameters;
            
            MeteorologicalStation();
            void destroy();
        };
        
        struct MeteorologicalConfiguration {
            int stationsLength;
            SimulationDataType::MeteorologicalStation *stations;
            
            MeteorologicalConfiguration();
            void destroy();
        };

        struct OutputParameter {
            int nameLength;
            char *name;
            
            OutputParameter();
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

            RecoveryVariables();
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
            SimulationDataType::WaterQualityConfiguration *waterQualityConfiguration;
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
            SimulationDataType::RecoveryVariables *recoveryVariables;
            int statusCode;
            int progress;
            
            Simulation();
            void destroy();
        };
    }
}

#endif // SIMULATION_DATA_TYPE_H
