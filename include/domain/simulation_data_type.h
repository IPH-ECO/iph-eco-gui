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
            
            void destroy() {
                delete xCoordinates;
                delete yCoordinates;
                delete northNeighbors;
                delete westNeighbors;
                delete southNeighbors;
                delete eastNeighbors;
                delete verticeIds;
            }
        };
        
        struct UnstructuredMesh {
			long long int numberOfPoints;
            double *xCoordinates;
            double *yCoordinates;
			long long int verticeIdsLength;
			long long int *verticeIds;
            
            void destroy() {
                delete xCoordinates;
                delete yCoordinates;
                delete verticeIds;
            }
        };
        
        struct GridData {
			long long int numberOfElements;
            double *weights;
            int typeId;
            
            void destroy() {
                delete weights;
            }
        };
        
        struct GridDataConfiguration {
            int numberOfLayers;
            SimulationDataType::GridData *layers;
            bool isStructured;
            SimulationDataType::StructuredMesh *structuredMesh;
            SimulationDataType::UnstructuredMesh *unstructuredMesh;
            
            void destroy() {
                layers->destroy();
                delete layers;

                if (structuredMesh) {
                    structuredMesh->destroy();
                    delete structuredMesh;
                }
                
                if (unstructuredMesh) {
                    unstructuredMesh->destroy();
                    delete unstructuredMesh;
                }
            }
        };
        
        struct HydrodynamicParameter {
            char *name;
            int length;
            double value;
            
            void destroy() {
                delete name;
            }
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
            
            void destroy() {
                delete cells;
                delete timeSeriesList;
            }
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
            
            void destroy() {
                delete name;
                delete timeSeriesList;
            }
        };
        
        struct MeteorologicalStation {
            double utmX;
            double utmY;
            int parametersLength;
            SimulationDataType::MeteorologicalParameter *parameters;
            
            void destroy() {
                parameters->destroy();
                delete parameters;
            }
        };
        
        struct MeteorologicalConfiguration {
            int stationsLength;
            SimulationDataType::MeteorologicalStation *stations;
            
            void destroy() {
                stations->destroy();
                delete stations;
            }
        };

        struct HydrodynamicConfiguration {
            int numberOfParameters;
            SimulationDataType::HydrodynamicParameter *parameters;
            int numberOfBoundaryConditions;
            SimulationDataType::BoundaryCondition *boundaryConditions;
            SimulationDataType::GridDataConfiguration *gridDataConfiguration;
            
            void destroy() {
                parameters->destroy();
                boundaryConditions->destroy();
                gridDataConfiguration->destroy();
                
                delete parameters;
                delete boundaryConditions;
                delete gridDataConfiguration;
            }
        };

        struct OutputParameter {
            int nameLength;
            char *name;
            
            void destroy() {
                delete name;
            }
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
            
            void destroy() {
                delete u;
                delete w;
                delete eta;
            }
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
            SimulationDataType::RecoveryVariables *recoveryVariables;
            int statusCode;
            int progress;
            
            void destroy() {
                hydrodynamicConfiguration->destroy();
                meteorologicalConfiguration->destroy();
                outputParameters->destroy();
                recoveryVariables->destroy();
                
                delete label;
                delete layers;
                delete hydrodynamicConfiguration;
                delete meteorologicalConfiguration;
                delete observations;
                delete outputDirectory;
                delete outputParameters;
                delete recoveryVariables;
            }
        };
    }
}

#endif // SIMULATION_DATA_TYPE_H
